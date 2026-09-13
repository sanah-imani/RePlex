#include "plugin_manager.hpp"
#include <iostream>

namespace fs = std::filesystem;

PluginManager::PluginManager(HostAPI* api): m_api(api) {}

void PluginManager::load(const std::string& path){
    auto loader = std::make_unique<PluginLoader>(path);
    if (loader->load(m_api)){
        m_plugins.push_back(std::move(loader));
    }
}

void PluginManager::updateAll(float dt){
    for (auto& loader: m_plugins){
        loader->update(dt);
    }
}

void PluginManager::unloadAll(){
    for (auto& loader : m_plugins){
        loader->unload();
    }
    m_plugins.clear();
}

void PluginManager::reloadAll(const std::string& sourcePath, const std::string& livePath){
    for (auto& loader : m_plugins){
        // unload serializes state into loader's buffer before dlclose
        loader->unload();
        // copy after unload so livePath is no longer dlopen'd
        fs::copy_file(sourcePath, livePath, fs::copy_options::overwrite_existing);
        // load restores state from buffer after init
        loader->load(m_api);
    }
}