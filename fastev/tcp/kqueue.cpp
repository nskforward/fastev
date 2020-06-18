#ifdef __APPLE__
#include "kqueue.hpp"

namespace fastev
{
    Reactor::Reactor()
    {
        event_base = kqueue();
        if (event_base == -1)
        {
            throw KernelException("cannot create event base kqueue");
        }
        registrySignal();
        Logger::log(LogLevel::INFO, "event method is kqueue");
    }

    Reactor::~Reactor()
    {
        close(event_base);
    }

    void Reactor::registryEvent(struct kevent *event)
    {
        if (kevent(event_base, event, 1, NULL, 0, NULL) == -1)
        {
            throw KernelException("cannot registry an event");
        }
        if ((*event).flags & EV_ERROR)
        {
            throw KernelException("registered event returned an error: %s", strerror((*event).data));
        }
    }

    void Reactor::onTimer(std::function<void()> func)
    {
        timer_cb = func;
        registryTimer(10);
    }

    void Reactor::registrySignal()
    {
        struct kevent ev;
        EV_SET(&ev, SIGINT, EVFILT_SIGNAL, EV_ADD | EV_ENABLE, 0, 0, NULL);
        signal(SIGINT, SIG_IGN);
        registryEvent(&ev);
    }

    void Reactor::registryTimer(time_t seconds)
    {
        struct kevent ev;
        EV_SET(&ev, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, seconds * 1000, 0);
        registryEvent(&ev);
    }

    void Reactor::watchRead(int fd)
    {
        struct kevent ev;
        EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        registryEvent(&ev);
    }

    void Reactor::unwatch(int fd)
    {
        struct kevent tmp_ev;
        EV_SET(&tmp_ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        registryEvent(&tmp_ev);
    }

    // START LOOP
    void Reactor::start()
    {
        struct kevent active_events[FASTEV_REACTOR_POLL_SIZE];
        struct timespec kqTimeout = {2, 0}; // 2s
        while (active)
        {
            int found_events = kevent(event_base, NULL, 0, active_events, FASTEV_REACTOR_POLL_SIZE, &kqTimeout);
            if (found_events == 0)
            {
                continue; // timeout
            }
            if (found_events < 0)
            {
                break;
            }
            for (int i = 0; i < found_events; i++)
            {
                // SIGNAL
                if (active_events[i].filter == EVFILT_SIGNAL)
                {
                    Logger::log(LogLevel::INFO, "signal %d", active_events[i].ident);
                    active = false;
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
                    onSocketEvent(active_events[i].ident);
                    continue;
                }
                Logger::log(LogLevel::ERROR, "unknown kqueue event: %d", active_events[i].filter);
            }
        }
    }
} // namespace fastev
#endif