#pragma once
#include <string>
#include <functional>
#include <atomic>

// Watches a single file for modifications using inotify.
// Call watch() on a background thread; it blocks until stop() is called.
class FileWatcher {
public:
    explicit FileWatcher(const std::string& watchPath, std::function<void()> onChanged);
    ~FileWatcher();

    FileWatcher(const FileWatcher&) = delete;
    FileWatcher& operator=(const FileWatcher&) = delete;
    // Blocks — run on a background thread.
    void watch();

    void stop();

private:
    std::string m_dir;
    std::string m_watchPath;
    std::function<void()> m_onChanged;
    int m_inotifyFd = -1;
    int m_watchFd   = -1;
    int m_stopFd = -1;

    std::atomic<bool> m_stopRequest{false};
};
