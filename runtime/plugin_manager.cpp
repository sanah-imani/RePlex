#include "plugin_manager.hpp"
#include <iostream>

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

void PluginManager::reloadAll(){
    for (auto& loader : m_plugins){
        loader->unload();
        loader->load(m_api);
    }
}