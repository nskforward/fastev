#if defined(__FreeBSD__) || defined(__APPLE__)

#ifndef FASTEV_KQUEUE_HPP
#define FASTEV_KQUEUE_HPP

#include <sys/event.h>
#include <unistd.h>
#include "../core/logger.hpp"
#include "../core/exception.hpp"
#include "../core/byte_buffer.hpp"

#ifndef FASTEV_REACTOR_POLL_SIZE
#define FASTEV_REACTOR_POLL_SIZE 256
#endif

using namespace std;

namespace fastev
{
    class Reactor
    {
    private:
        bool _active = true;
        int _event_base;
        function<void()> timer_cb;
        function<void(int fd)> read_cb;
        function<void(int fd)> write_cb;

        void registryEvent(struct kevent *ev);
        void registrySignal();

    public:
        Reactor();
        ~Reactor();
        void start();
        void watchRead(int fd);
        void watchWrite(int fd);
        void unwatchRead(int fd);
        void unwatchWrite(int fd);
        void onTimer(function<void()> func);
        void onRead(function<void(int fd)> func);
        void onWrite(function<void(int fd)> func);
        void registryTimer(time_t seconds);
    };
} // namespace fastev

#endif
#endif