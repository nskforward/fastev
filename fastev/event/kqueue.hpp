#ifndef FASTEV_KQUEUE_HPP
#define FASTEV_KQUEUE_HPP

#include <sys/event.h>
#include <unistd.h>
#include "../core/logger.hpp"
#include "../core/exception.hpp"

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
        struct kevent _ev;
        function<void()> timer_cb;

        void registryEvent();
        void registrySignal();

    public:
        Reactor();
        ~Reactor();
        void start(function<void(int fd)> callback);
        void watch(int fd);
        void unwatch(int fd);
        void onTimer(function<void()> func);
        void registryTimer(time_t seconds);
    };
} // namespace fastev

#endif