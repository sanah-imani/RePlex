#pragma once

#include <vector>
#include <string>
#include <memory>
#include "../host/host_api.hpp"
#include "../host/plugin_loader.hpp"

class PluginManager {
public:
    explicit PluginManager(HostAPI* api);

    void load(const std::string& path);
    void unloadAll();
    void updateAll(float dt);
    void reloadAll();

private:
    HostAPI* m_api;
    std::vector<std::unique_ptr<PluginLoader>> m_plugins;
};