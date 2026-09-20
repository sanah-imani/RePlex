#include "watcher.hpp"

#include <sys/inotify.h>
#include <sys/eventfd.h>
#include <linux/limits.h>
#include <unistd.h>
#include <poll.h>
#include <filesystem>
#include <cstdint>
#include <cerrno>
#include <iostream>

FileWatcher::FileWatcher(const std::string& watchPath, std::function<void()> onChanged)
    : m_watchPath(watchPath), m_onChanged(std::move(onChanged))
{
    std::filesystem::path p(watchPath);
    m_dir = p.parent_path().empty() ? "." : p.parent_path().string();

    // Created here, not in watch(), so stop() can signal even if it races
    // ahead of the watcher thread actually reaching the poll loop.
    m_stopFd = eventfd(0, EFD_CLOEXEC);
    if (m_stopFd < 0) std::cerr << "eventfd failed for: " << m_watchPath << "\n";
}

FileWatcher::~FileWatcher()
{
    stop();
    if (m_stopFd >= 0) { close(m_stopFd); m_stopFd = -1; }
}

void FileWatcher::watch()
{
    m_inotifyFd = inotify_init();
    if (m_inotifyFd < 0) {
        std::cerr << "inotify_init failed\n";
        return;
    }

    // Watch the parent directory, not the file itself: linkers replace .so
    // files via rename or unlink+create, which yields a new inode — an
    // inode-based watch goes permanently deaf after the first rebuild.
    m_watchFd = inotify_add_watch(m_inotifyFd, m_dir.c_str(),
                                  IN_CLOSE_WRITE | IN_MOVED_TO | IN_CREATE);
    if (m_watchFd < 0) {
        std::cerr << "inotify_add_watch failed for: " << m_dir << "\n";
        close(m_inotifyFd);
        m_inotifyFd = -1;
        return;
    }

    const std::string filename = std::filesystem::path(m_watchPath).filename().string();

    constexpr size_t BUF_SIZE = 16 * (sizeof(inotify_event) + NAME_MAX + 1);
    alignas(inotify_event) char buf[BUF_SIZE];
    bool pending = false;

    while (!m_stopRequest) {
        pollfd fds[2] = { { m_inotifyFd, POLLIN, 0 }, { m_stopFd, POLLIN, 0 } };

        // Block until something happens; once an event lands, wait for a quiet
        // gap instead so one build's burst of events collapses into one reload.
        int n = poll(fds, 2, pending ? 150 : -1);
        if (n < 0) {
            if (errno == EINTR) continue;
            break;
        }
        if (n == 0) { pending = false; m_onChanged(); continue; }
        if (fds[1].revents & POLLIN) break;   // stop() was called

        ssize_t len = read(m_inotifyFd, buf, BUF_SIZE);
        if (len <= 0) break;

        for (char* p = buf; p < buf + len; ) {
            auto* ev = reinterpret_cast<inotify_event*>(p);
            if (ev->len > 0 && filename == ev->name) pending = true;
            p += sizeof(inotify_event) + ev->len;
        }
    }

    // watch() owns these fds, so stop() can never close them out from under
    // a blocked read() on another thread.
    if (m_watchFd >= 0)   { inotify_rm_watch(m_inotifyFd, m_watchFd); m_watchFd = -1; }
    if (m_inotifyFd >= 0) { close(m_inotifyFd); m_inotifyFd = -1; }
}

void FileWatcher::stop()
{
    m_stopRequest = true;
    if (m_stopFd >= 0) {
        uint64_t one = 1;
        ssize_t r = write(m_stopFd, &one, sizeof(one));
        (void)r;   // nothing useful to do if the wake write fails
    }
}
