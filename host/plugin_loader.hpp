#pragma once
#include <string>

class PluginLoader {
    public:
        explicit PluginLoader(const std::string& libraryPath);

        bool load();
        void unload();

        bool isLoaded() const;

        void update(float dt);
    
    private:
        std::string m_libraryPath;
        void* m_handle;

        using UpdateFunc = void (*)(float);
        UpdateFunc m_update;
};