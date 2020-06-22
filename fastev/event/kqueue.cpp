#include "kqueue.hpp"

namespace fastev
{
    Reactor::Reactor()
    {
        _event_base = kqueue();
        if (_event_base == -1)
        {
            throw KernelException("cannot create event base kqueue");
        }
        registrySignal();
    }

    Reactor::~Reactor()
    {
        close(_event_base);
    }

    void Reactor::registrySignal()
    {
        EV_SET(&_ev, SIGINT, EVFILT_SIGNAL, EV_ADD | EV_ENABLE, 0, 0, NULL);
        signal(SIGINT, SIG_IGN);
        registryEvent();
    }

    void Reactor::onTimer(std::function<void()> func)
    {
        timer_cb = func;
        registryTimer(10);
    }

    void Reactor::registryTimer(time_t seconds)
    {
        EV_SET(&_ev, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, seconds, 0);
        registryEvent();
    }

    void Reactor::registryEvent()
    {
        if (kevent(_event_base, &_ev, 1, NULL, 0, NULL) == -1)
        {
            throw KernelException("cannot registry an event");
        }
        if (_ev.flags & EV_ERROR)
        {
            throw KernelException("registered event returned an error: %s", strerror(_ev.data));
        }
    }

    void Reactor::watch(int fd)
    {
        EV_SET(&_ev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        registryEvent();
    }

    void Reactor::watch(int fd, void *buff)
    {
        EV_SET(&_ev, fd, EVFILT_READ, EV_ADD, 0, 0, buff);
        registryEvent();
    }

    void Reactor::unwatch(int fd)
    {
        EV_SET(&_ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        registryEvent();
    }

    // START LOOP
    void Reactor::start(function<void(int fd, void *buff)> callback)
    {
        //Logger::log(LogLevel::INFO, "event lopp with method kqueue is running");
        struct kevent active_events[FASTEV_REACTOR_POLL_SIZE];
        struct timespec kqTimeout = {5, 0}; // 5s
        while (_active)
        {
            int found_events = kevent(_event_base, NULL, 0, active_events, FASTEV_REACTOR_POLL_SIZE, &kqTimeout);
            if (found_events == 0)
            {
                continue; // timeout
            }
            if (found_events < 0)
            {
                break; // error
            }
            for (int i = 0; i < found_events; i++)
            {
                // SIGNAL
                if (active_events[i].filter == EVFILT_SIGNAL)
                {
                    //Logger::log(LogLevel::INFO, "signal %d", active_events[i].ident);
                    _active = false;
                    break;
                }
                // TIMER
                if (active_events[i].filter == EVFILT_TIMER)
                {
                    timer_cb();
                    continue;
                }
                // READ
                if (active_events[i].filter == EVFILT_READ)
                {
                    callback(active_events[i].ident, active_events[i].udata);
                    continue;
                }
                Logger::log(LogLevel::ERROR, "unknown kqueue event: %d", active_events[i].filter);
            }
        }
    }
} // namespace fastev