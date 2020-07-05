#ifndef FASTEV_EPOLL_HPP
#define FASTEV_EPOLL_HPP

#ifdef __linux__
#include <sys/epoll.h>
#include <functional>
#include "../core/logger.hpp"
#include "../core/exception.hpp"
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <unistd.h>

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
        int _sigint_fd;
        int _timer_fd;
        function<void()> timer_cb;
        function<void(int fd)> read_cb;
        function<void(int fd)> write_cb;
        int registrySignal();
        int registryTimer(time_t seconds);

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
    };
} // namespace fastev
#endif
#endif