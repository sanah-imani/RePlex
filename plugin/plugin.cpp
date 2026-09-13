#include "plugin.hpp"
#include <cstddef>
#include <cstring>
#include <cstdio>

// s_api is a translation-unit local pointer — starts null,
// set on init, cleared on shutdown.
static HostAPI* s_api = nullptr;

struct PluginState{
    int tickCount;
    float totalTime;
};
static PluginState s_state = {};

void init(HostAPI* api)
{
    s_api = api;
    s_api->log("Plugin init");
}

void update(float dt)
{
    if (!s_api) return;

    s_state.tickCount++;
    s_state.totalTime += dt;

    char msg[64];
    snprintf(msg, sizeof(msg), "tick=%d, time=%.3f", s_state.tickCount, s_state.totalTime);
    s_api->log(msg);
}

void shutdown()
{
    if (s_api) s_api->log("Plugin shutdown");
    s_api = nullptr;
}

void serialize(void *buffer, size_t *outSize)
{
    *outSize = sizeof(PluginState);
    memcpy(buffer, &s_state, sizeof(PluginState));
    if (s_api) s_api->log("Plugin state serialized");
}

void deserialize(const void* buffer, size_t size)
{
    if (size == sizeof(PluginState)){
        memcpy(&s_state, buffer, sizeof(PluginState));
        if (s_api) s_api->log("Plugin state restored");
    }
}