#pragma once

// ABI contract between host and plugin.
// Must remain pure C — no C++ types, no STL, no virtual functions.
// Both host and plugin include this file.
struct HostAPI {
    void (*log)(const char* msg);
};
