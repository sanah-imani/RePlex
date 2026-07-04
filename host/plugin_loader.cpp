#include "plugin_loader.hpp"

#include <dlfcn.h>
#include <iostream>

PluginLoader::PluginLoader(const std::string& libraryPath)
: m_libraryPath(libraryPath),
  m_handle(nullptr),
  m_update(nullptr)
{
}

bool PluginLoader::load()
{
    m_handle = dlopen(m_libraryPath.c_str(), RTLD_LAZY);

    if (!m_handle){
        std::cerr << "Failed to load library:\n",
                << dlerror() << '\n';
        return false;
    }

    dlerror(); // clear previous errors

    m_update = reinterpret_cast<UpdateFunc>(
        dlsym(m_handle, "update")
    );

    if (const char* error = dlerror())
    {
        std::cerr << "Failed to find symbol 'update':\n"
                    << error << '\n';

        dlclose(m_handle);
        m_handle = nullptr;
        m_update = nullptr;

        return false;
    }

    std::cout << "Plugin loaded successfully. \n";
    return true;
}

void PluginLoader::unload()
{
    if (m_handle){
        dlclose(m_handle);

        m_handle = nullptr;
        m_update = nullptr;

        std::cout << "Plugin unloaded.\n";
    }
}

bool PluginLoader::isLoaded() const
{
    return m_handle != nullptr;
}

void PluginLoader::update(float dt)
{
    if (m_update)
    {
        m_update(dt);
    }
}