#pragma once

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

#include "../host/host_api.hpp"
#include <cstddef>

extern "C" {
    EXPORT void init(HostAPI* api);
    EXPORT void update(float dt);
    EXPORT void shutdown();

    // Called before unloading - writing state into buffer
    // outSize: plugin tells host how many bytes it wrote
    EXPORT void serialize(void* buffer, size_t* outSize);

    // called after load - host frees up the buffer from before
    EXPORT void deserialize(const void* buffer, size_t size);
}
