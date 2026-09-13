#pragma once

#include <vector>
#include <string>
#include <memory>
#include <filesystem>
#include "../host/host_api.hpp"
#include "../host/plugin_loader.hpp"

class PluginManager {
public:
    explicit PluginManager(HostAPI* api);

    void load(const std::string& path);
    void unloadAll();
    void updateAll(float dt);

    // Reloads all plugins in-place — same loader instances so state buffers survive
    void reloadAll(const std::string& sourcePath, const std::string& livePath);

private:
    HostAPI* m_api;
    std::vector<std::unique_ptr<PluginLoader>> m_plugins;
};