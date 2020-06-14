#ifndef FASTEV_KQUEUE_HPP
#define FASTEV_KQUEUE_HPP

#include <sys/event.h>
#include <unistd.h>
#include <functional>
#include "../core/logger.hpp"
#include "../core/exception.hpp"

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

        std::function<void(int fd)> on_read_cb;
        std::function<void()> on_timer_cb;
        void registry_event(struct kevent *event);
        void registry_signal_event();
        void registry_timer_event(time_t seconds);

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