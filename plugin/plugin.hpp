#pragma once

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include "../host/host_api.hpp"

extern "C" {
    EXPORT void init(HostAPI* api);
    EXPORT void update(float dt);
    EXPORT void shutdown();
}
