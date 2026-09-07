#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <atomic>
#include <mutex>
#include "host_api.hpp"
#include "../runtime/plugin_manager.hpp"
#include "../runtime/watcher.hpp"

namespace fs = std::filesystem;

static const std::string kSourceLib = "./build/libplugin.so";
static const std::string kLiveLib   = "./build/libplugin_live.so";

// The actual implementation of the log service.
// Must be a free function — raw function pointers can't hold captures or state.
static void hostLog(const char* msg)
{
    std::cout << "[plugin] " << msg << "\n";
}

int main() {
    std::cout << "Replex host starting...\n";

    // Build the API table — host owns this for the entire session.
    HostAPI api;
    api.log = hostLog;

    // Copy .so before loading so rebuilds can overwrite the original freely.
    fs::copy_file(kSourceLib, kLiveLib, fs::copy_options::overwrite_existing);

    PluginManager manager(&api);
    manager.load(kLiveLib);

    std::mutex loaderMutex;
    std::atomic<bool> reloadPending = false;

    // Watch the original .so — fires when a rebuild overwrites it.
    FileWatcher watcher(kSourceLib, [&]() {
        reloadPending = true;
    });

    std::thread watchThread([&]() { watcher.watch(); });

    while (true) {
        if (reloadPending.exchange(false)) {
            std::cout << "Change detected -- reloading plugin...\n";
            std::lock_guard<std::mutex> lock(loaderMutex);
            // unload first so kLiveLib is no longer dlopen'd, then overwrite it
            manager.unloadAll();
            fs::copy_file(kSourceLib, kLiveLib, fs::copy_options::overwrite_existing);
            manager.load(kLiveLib);
        }

        {
            std::lock_guard<std::mutex> lock(loaderMutex);
            manager.updateAll(0.016f);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    watcher.stop();
    watchThread.join();
    manager.unloadAll(); 
    return 0;
}
