#if defined(__FreeBSD__) || defined(__APPLE__)

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
        registrySignal(SIGINT);
        registrySignal(SIGTERM);
    }

    Reactor::~Reactor()
    {
        close(_event_base);
    }

    void Reactor::registrySignal(int signum)
    {
        struct kevent ev;
        EV_SET(&ev, signum, EVFILT_SIGNAL, EV_ADD | EV_ENABLE, 0, 0, NULL);
        signal(signum, SIG_IGN);
        registryEvent(&ev);
    }

    void Reactor::onTimer(std::function<void()> func)
    {
        timer_cb = func;
        registryTimer(10);
    }

    void Reactor::onRead(function<void(int fd)> func)
    {
        read_cb = func;
    }

    void Reactor::onWrite(function<void(int fd)> func)
    {
        write_cb = func;
    }

    void Reactor::registryTimer(time_t seconds)
    {
        struct kevent ev;
        EV_SET(&ev, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE, NOTE_SECONDS, seconds, 0);
        registryEvent(&ev);
    }

    void Reactor::registryEvent(struct kevent *ev)
    {
        if (kevent(_event_base, ev, 1, NULL, 0, NULL) == -1)
        {
            if (errno == ENOENT)
            {
                throw KernelException("reactor: The event could	not be found to	be modified or deleted");
            }
            throw KernelException("error #%d: cannot registry an event", errno);
        }
        if (ev->flags & EV_ERROR)
        {
            throw KernelException("registered event returned an error: %s", strerror(ev->data));
        }
    }

    void Reactor::watchRead(int fd)
    {
        struct kevent ev;
        EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        registryEvent(&ev);
    }

    void Reactor::watchWrite(int fd)
    {
        struct kevent ev;
        EV_SET(&ev, fd, EVFILT_WRITE, EV_ADD, 0, 0, NULL);
        registryEvent(&ev);
    }

    void Reactor::unwatchRead(int fd)
    {
        struct kevent ev;
        EV_SET(&ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        registryEvent(&ev);
    }

    void Reactor::unwatchWrite(int fd)
    {
        struct kevent ev;
        EV_SET(&ev, fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
        registryEvent(&ev);
    }

    // START LOOP
    void Reactor::start()
    {
        if (read_cb == NULL)
        {
            throw KernelException("Reactor onRead callback is not defined");
        }
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
                    read_cb(active_events[i].ident);
                    continue;
                }
                // WRITE
                if (active_events[i].filter == EVFILT_WRITE)
                {
                    write_cb(active_events[i].ident);
                    continue;
                }
                Logger::log(LogLevel::ERROR, "unknown kqueue event: %d", active_events[i].filter);
            }
        }
    }
} // namespace fastev

#endif