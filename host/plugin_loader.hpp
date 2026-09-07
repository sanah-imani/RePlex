#pragma once
#include <string>
#include "host_api.hpp"

class PluginLoader {
public:
    explicit PluginLoader(const std::string& libraryPath);

    // api* passed at load time so hot-reloads can use the same API instance
    bool load(HostAPI* api);
    void unload();

    bool isLoaded() const;

    void update(float dt);

private:
    std::string m_libraryPath;
    void* m_handle = nullptr;

    using InitFunc     = void (*)(HostAPI*);
    using UpdateFunc   = void (*)(float);
    using ShutdownFunc = void (*)();

    InitFunc     m_init     = nullptr;
    UpdateFunc   m_update   = nullptr;
    ShutdownFunc m_shutdown = nullptr;
};
