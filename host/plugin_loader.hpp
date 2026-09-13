#pragma once
#include <array>
#include <cstddef>
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
    using SerializeFunc = void (*) (void*, size_t*);
    using DeserializeFunc = void(*)(const void*, size_t);

    InitFunc     m_init     = nullptr;
    UpdateFunc   m_update   = nullptr;
    ShutdownFunc m_shutdown = nullptr;
    SerializeFunc m_serialize = nullptr;
    DeserializeFunc m_deserialize = nullptr;

    static constexpr size_t kMaxStateSize = 1024;
    std::array<std::byte, kMaxStateSize> m_stateBuffer;
    size_t m_stateSize = 0;
    bool m_hasState = false;
};
