#include "manifest.hpp"
#include "../third_party/nlohmann/json.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

std::vector<PluginEntry> loadManifest(const std::string& manifestPath)
{
    std::vector<PluginEntry> entries;

    std::ifstream file(manifestPath);
    if (!file.is_open()) {
        std::cerr << "Failed to open manifest: " << manifestPath << "\n";
        return entries;
    }

    json data;
    try {
        file >> data;
    }
    catch (const json::parse_error& e) {
        std::cerr << "Manifest parse error: " << e.what() << "\n";
        return entries;
    }

    for (const auto& item : data.at("plugins")) {
        PluginEntry entry;
        entry.name  = item.at("name").get<std::string>();
        entry.path  = item.at("path").get<std::string>();
        entry.watch = item.value("watch", false);
        entries.push_back(std::move(entry));
    }

    std::cout << "Manifest loaded: " << entries.size() << " plugin(s)\n";
    return entries;
}
