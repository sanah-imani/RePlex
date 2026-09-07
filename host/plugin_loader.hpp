#pragma once
#include <string>

class PluginLoader {
public:
    explicit PluginLoader(const std::string& libraryPath);

    bool load();
    void unload();

    bool isLoaded() const;

    void init();
    void update(float dt);
    void shutdown();

private:
    std::string m_libraryPath;
    void* m_handle = nullptr;

    using InitFunc     = void (*)();
    using UpdateFunc   = void (*)(float);
    using ShutdownFunc = void (*)();

    InitFunc     m_init     = nullptr;
    UpdateFunc   m_update   = nullptr;
    ShutdownFunc m_shutdown = nullptr;
};
