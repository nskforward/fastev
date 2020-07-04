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
#define FASTEV_REACTOR_POLL_SIZE 128
#endif

using namespace std;

namespace fastev
{
    class Reactor
    {
    private:
        bool active = true;
        int event_base;
        int sigint_fd;
        int timer_fd;
        function<void()> timer_cb;
        int registrySignal();
        int registryTimer(time_t seconds);
        virtual void onSocketEvent(int fd, void *ptr) = 0;

    public:
        Reactor();
        ~Reactor();
        void start();
        void watchRead(int fd, void *ptr);
        void watchMaster(int fd);
        void unwatch(int fd);
        void onTimer(function<void()> func);
    };
} // namespace fastev
#endif
#endif