#ifndef FASTEV_KQUEUE_HPP
#define FASTEV_KQUEUE_HPP

#include <sys/event.h>
#include <unistd.h>
#include <functional>
#include "../core/logger.hpp"
#include "../core/exception.hpp"

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
        function<void()> timer_cb;

        void registryEvent(struct kevent *event);
        void registrySignal();
        void registryTimer(time_t seconds);
        virtual void onSocketEvent(int fd) = 0;

    public:
        Reactor();
        ~Reactor();
        void start();
        void watchRead(int fd);
        void unwatch(int fd);
        void onTimer(function<void()> func);
    };
} // namespace fastev

#endif