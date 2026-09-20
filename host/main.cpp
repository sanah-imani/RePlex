#include <iostream>
#include <thread>
#include <chrono>
#include "host_api.hpp"
#include "../runtime/manifest.hpp"
#include "../runtime/plugin_manager.hpp"

static void hostLog(const char* msg)
{
    std::cout << "[plugin] " << msg << "\n";
}

int main() {
    std::cout << "Replex host starting...\n";

    HostAPI api;
    api.log = hostLog;

    auto entries = loadManifest("./plugins.json");
    if (entries.empty()) {
        std::cerr << "No plugins loaded — check plugins.json\n";
        return 1;
    }

    PluginManager manager(&api);
    for (const auto& entry : entries)
        manager.load(entry);

    while (true) {
        manager.updateAll(0.016f);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    manager.unloadAll();
    return 0;
}
