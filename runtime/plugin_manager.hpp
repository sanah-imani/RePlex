#pragma once

#include <vector>
#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <filesystem>
#include "../host/host_api.hpp"
#include "../host/plugin_loader.hpp"
#include "manifest.hpp"
#include "watcher.hpp"

class PluginManager {
public:
    explicit PluginManager(HostAPI* api);
    ~PluginManager();

    // Load a plugin from a manifest entry — sets up its own FileWatcher if watch=true
    void load(const PluginEntry& entry);

    void unloadAll();
    void updateAll(float dt);

private:
    struct ManagedPlugin {
        std::string                   sourcePath;
        std::string                   livePath;
        std::unique_ptr<PluginLoader> loader;
        std::unique_ptr<FileWatcher>  watcher;
        std::thread                   watchThread;
        std::atomic<bool>             reloadPending{false};

        // non-copyable, non-movable due to atomic + thread — constructed in place
        ManagedPlugin() = default;
        ManagedPlugin(const ManagedPlugin&) = delete;
        ManagedPlugin& operator=(const ManagedPlugin&) = delete;
    };

    void doReload(ManagedPlugin& p);

    HostAPI* m_api;
    std::mutex m_mutex;
    std::vector<std::unique_ptr<ManagedPlugin>> m_plugins;
};
