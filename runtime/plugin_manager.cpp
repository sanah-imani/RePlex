#include "plugin_manager.hpp"
#include <iostream>

namespace fs = std::filesystem;

// Derive live path from source: libplugin.so -> libplugin_live.so
static std::string makeLivePath(const std::string& sourcePath)
{
    fs::path p(sourcePath);
    std::string stem = p.stem().string();       // e.g. "libplugin"
    std::string ext  = p.extension().string();  // e.g. ".so"
    return (p.parent_path() / (stem + "_live" + ext)).string();
}

PluginManager::PluginManager(HostAPI* api) : m_api(api) {}

PluginManager::~PluginManager()
{
    unloadAll();
}

void PluginManager::load(const PluginEntry& entry)
{
    auto mp = std::make_unique<ManagedPlugin>();
    mp->sourcePath = entry.path;
    mp->livePath   = makeLivePath(entry.path);

    // copy .so before loading so builds can overwrite the original freely
    fs::copy_file(mp->sourcePath, mp->livePath, fs::copy_options::overwrite_existing);

    mp->loader = std::make_unique<PluginLoader>(mp->livePath);
    if (!mp->loader->load(m_api)) {
        std::cerr << "Failed to load plugin: " << entry.name << "\n";
        return;
    }

    if (entry.watch) {
        // capture raw pointer — ManagedPlugin outlives the watcher thread
        ManagedPlugin* raw = mp.get();
        mp->watcher = std::make_unique<FileWatcher>(mp->sourcePath, [raw]() {
            raw->reloadPending = true;
        });
        mp->watchThread = std::thread([raw]() { raw->watcher->watch(); });
    }

    m_plugins.push_back(std::move(mp));
    std::cout << "Loaded plugin: " << entry.name << "\n";
}

void PluginManager::doReload(ManagedPlugin& p)
{
    std::cout << "Reloading plugin...\n";
    p.loader->unload();
    fs::copy_file(p.sourcePath, p.livePath, fs::copy_options::overwrite_existing);
    p.loader->load(m_api);
}

void PluginManager::updateAll(float dt)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    for (auto& mp : m_plugins) {
        if (mp->reloadPending.exchange(false)) {
            doReload(*mp);
        }
        mp->loader->update(dt);
    }
}

void PluginManager::unloadAll()
{
    for (auto& mp : m_plugins) {
        if (mp->watcher) {
            mp->watcher->stop();
            if (mp->watchThread.joinable())
                mp->watchThread.join();
        }
        mp->loader->unload();
    }
    m_plugins.clear();
}
