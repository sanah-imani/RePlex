#include <iostream>
#include <thread>
#include <chrono>
#include <filesystem>
#include <atomic>
#include <mutex>
#include "host_api.hpp"
#include "plugin_loader.hpp"
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

    PluginLoader loader(kLiveLib);
    if (!loader.load(&api)) return 1;

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
            loader.unload();
            fs::copy_file(kSourceLib, kLiveLib, fs::copy_options::overwrite_existing);
            loader.load(&api);  // same API instance, new plugin code
        }

        {
            std::lock_guard<std::mutex> lock(loaderMutex);
            loader.update(0.016f);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    watcher.stop();
    watchThread.join();
    loader.unload();
    return 0;
}
