#ifndef FASTEV_EPOLL_HPP
#define FASTEV_EPOLL_HPP

#ifdef __linux__
#include <sys/epoll.h>
#include <functional>
#include "logger.hpp"
#include "exception.hpp"
#include <signal.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>

#ifndef FASTEV_REACTOR_POLL_SIZE
#define FASTEV_REACTOR_POLL_SIZE 16
#endif

namespace fastev
{
    class Reactor
    {
    private:
        bool active = true;
        int event_base;
        int sigint_fd;
        int timer_fd;
        std::function<void(int fd)> on_read_cb;
        std::function<void()> on_timer_cb;
        int registry_signal_event();
        int registry_timer_event(time_t seconds);

    public:
        Reactor();
        ~Reactor();
        void start();
        void registry_read_event_fd(int fd);
        void unregistry_read_event_fd(int fd);
        void registry_callback_on_read(std::function<void(int fd)> func);
        void registry_callback_on_timer(std::function<void()> func);
    };
} // namespace fastev
#endif
#endif