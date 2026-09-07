#include "plugin_loader.hpp"

#include <dlfcn.h>
#include <iostream>

PluginLoader::PluginLoader(const std::string& libraryPath)
    : m_libraryPath(libraryPath)
{
}

bool PluginLoader::load()
{
    m_handle = dlopen(m_libraryPath.c_str(), RTLD_LAZY);

    if (!m_handle) {
        std::cerr << "Failed to load library: " << dlerror() << '\n';
        return false;
    }

    dlerror(); // clear previous errors

    // Required symbol
    m_update = reinterpret_cast<UpdateFunc>(dlsym(m_handle, "update"));
    if (const char* error = dlerror()) {
        std::cerr << "Failed to find symbol 'update': " << error << '\n';
        dlclose(m_handle);
        m_handle = nullptr;
        return false;
    }

    // Optional symbols — silently skip if not exported
    m_init = reinterpret_cast<InitFunc>(dlsym(m_handle, "init"));
    dlerror(); // clear any error from optional lookup

    m_shutdown = reinterpret_cast<ShutdownFunc>(dlsym(m_handle, "shutdown"));
    dlerror(); // clear any error from optional lookup

    std::cout << "Plugin loaded successfully.\n";

    // Call init if the plugin exports it
    if (m_init) m_init();

    return true;
}

void PluginLoader::unload()
{
    if (m_handle) {
        // Call shutdown if the plugin exports it
        if (m_shutdown) m_shutdown();

        dlclose(m_handle);

        m_handle   = nullptr;
        m_init     = nullptr;
        m_update   = nullptr;
        m_shutdown = nullptr;

        std::cout << "Plugin unloaded.\n";
    }
}

bool PluginLoader::isLoaded() const
{
    return m_handle != nullptr;
}

void PluginLoader::init()
{
    if (m_init) m_init();
}

void PluginLoader::update(float dt)
{
    if (m_update) m_update(dt);
}

void PluginLoader::shutdown()
{
    if (m_shutdown) m_shutdown();
}
