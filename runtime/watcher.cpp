#include "watcher.hpp"

#include <sys/inotify.h>
#include <linux/limits.h>
#include <unistd.h>
#include <iostream>

FileWatcher::FileWatcher(const std::string& watchPath, std::function<void()> onChanged)
    : m_watchPath(watchPath), m_onChanged(std::move(onChanged))
{
}

FileWatcher::~FileWatcher()
{
    stop();
}

void FileWatcher::watch()
{
    m_inotifyFd = inotify_init();
    if (m_inotifyFd < 0) {
        std::cerr << "inotify_init failed\n";
        return;
    }

    m_watchFd = inotify_add_watch(m_inotifyFd, m_watchPath.c_str(), IN_CLOSE_WRITE);
    if (m_watchFd < 0) {
        std::cerr << "inotify_add_watch failed for: " << m_watchPath << "\n";
        close(m_inotifyFd);
        m_inotifyFd = -1;
        return;
    }

    m_running = true;

    constexpr size_t BUF_SIZE = sizeof(inotify_event) + NAME_MAX + 1;
    char buf[BUF_SIZE];

    while (m_running) {
        ssize_t len = read(m_inotifyFd, buf, BUF_SIZE);
        if (len < 0) break;

        m_onChanged();
    }
}

void FileWatcher::stop()
{
    m_running = false;
    if (m_watchFd >= 0)   { inotify_rm_watch(m_inotifyFd, m_watchFd); m_watchFd = -1; }
    if (m_inotifyFd >= 0) { close(m_inotifyFd); m_inotifyFd = -1; }
}
