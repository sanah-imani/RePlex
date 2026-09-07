#include "plugin.hpp"

// s_api is a translation-unit local pointer — starts null,
// set on init, cleared on shutdown.
static HostAPI* s_api = nullptr;

void init(HostAPI* api)
{
    s_api = api;
    s_api->log("Plugin init");
}

void update(float dt)
{
    if (s_api) s_api->log("Plugin running");
}

void shutdown()
{
    if (s_api) s_api->log("Plugin shutdown");
    s_api = nullptr;
}
