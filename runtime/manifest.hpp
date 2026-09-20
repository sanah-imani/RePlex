#pragma once

#include <string>
#include <vector>

struct PluginEntry {
    std::string name;
    std::string path;
    bool watch;
};

std::vector<PluginEntry> loadManifest(const std::string& manifestPath);