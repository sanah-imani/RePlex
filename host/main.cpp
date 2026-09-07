#include <iostream>
#include <thread>
#include <chrono>
#include "plugin_loader.hpp"

int main() {
    std::cout << "Replex host starting...\n";

    PluginLoader loader("./build/libplugin.so");

    if (!loader.load()) {
        return 1;
    }

    while (true) {
        loader.update(0.016f);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    loader.unload();
    return 0;
}
