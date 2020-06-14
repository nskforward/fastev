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
        registry_signal_event();
        Logger::log(LogLevel::INFO, "event method is kqueue");
    }
    Reactor::~Reactor()
    {
        close(event_base);
    }

    void Reactor::registry_callback_on_read(std::function<void(int fd)> func)
    {
        on_read_cb = func;
    }

    void Reactor::registry_callback_on_timer(std::function<void()> func)
    {
        on_timer_cb = func;
    }

    void Reactor::registry_signal_event()
    {
        struct kevent ev;
        EV_SET(&ev, SIGINT, EVFILT_SIGNAL, EV_ADD | EV_ENABLE, 0, 0, NULL);
        signal(SIGINT, SIG_IGN);
        registry_event(&ev);
    }

    void Reactor::registry_timer_event(time_t seconds)
    {
        struct kevent ev;
        EV_SET(&ev, 1, EVFILT_TIMER, EV_ADD | EV_ENABLE, 0, seconds * 1000, 0);
        registry_event(&ev);
    }

    void Reactor::registry_read_event_fd(int fd)
    {
        struct kevent ev;
        EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
        registry_event(&ev);
    }

    void Reactor::unregistry_read_event_fd(int fd)
    {
        struct kevent tmp_ev;
        EV_SET(&tmp_ev, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
        registry_event(&tmp_ev);
    }

    // PRIVATE METHOD
    void Reactor::registry_event(struct kevent *event)
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

    // START LOOP
    void Reactor::start()
    {
        if (on_read_cb == NULL)
        {
            throw KernelException("'registry_callback_on_read()' function must be called before 'start()'");
        }
        if (on_timer_cb != NULL)
        {
            registry_timer_event(10);
        }
        struct kevent active_events[FASTEV_REACTOR_POLL_SIZE];
        while (active)
        {
            int found_events = kevent(event_base, NULL, 0, active_events, FASTEV_REACTOR_POLL_SIZE, NULL);
            //Logger::log(LogLevel::TRACE, "events: %d", found_events);
            if (found_events == 0)
            {
                throw KernelException("error code:%d on 'kevent()'", found_events);
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
                if (active_events[i].filter == EVFILT_TIMER)
                {
                    if (on_timer_cb != NULL)
                    {
                        on_timer_cb();
                    }
                    continue;
                }
                if (active_events[i].filter == EVFILT_READ)
                {
                    on_read_cb(active_events[i].ident);
                }
            }
        }
    }
} // namespace fastev
#endif