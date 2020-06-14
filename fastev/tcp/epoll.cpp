#include "epoll.hpp"

#ifdef __linux__
namespace fastev
{
    Reactor::Reactor()
    {
        event_base = epoll_create1(0);
        if (event_base == -1)
        {
            throw KernelException("cannot create event base epoll");
        }
        sigint_fd = registry_signal_event();
        Logger::log(LogLevel::INFO, "event method is epoll");
    }

    Reactor::~Reactor()
    {
        close(event_base);
    }

    void Reactor::registry_read_event_fd(int fd)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if (epoll_ctl(event_base, EPOLL_CTL_ADD, fd, &ev) == -1)
        {
            throw KernelException("cannot registry an event");
        }
    }

    void Reactor::unregistry_read_event_fd(int fd)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if (epoll_ctl(event_base, EPOLL_CTL_DEL, fd, &ev) == -1)
        {
            throw KernelException("cannot registry an event");
        }
    }

    void Reactor::registry_callback_on_read(std::function<void(int fd)> func)
    {
        on_read_cb = func;
    }

    void Reactor::registry_callback_on_timer(std::function<void()> func)
    {
        on_timer_cb = func;
    }

    int Reactor::registry_timer_event(time_t seconds)
    {
        struct itimerspec tspec;
        tspec.it_value.tv_sec = 1;
        tspec.it_value.tv_nsec = 0;
        tspec.it_interval.tv_sec = seconds;
        tspec.it_interval.tv_nsec = 0;
        int fd = timerfd_create(CLOCK_MONOTONIC, 0);
        if (fd == -1)
        {
            throw KernelException("timerfd_create() failed");
        }
        timerfd_settime(fd, TFD_TIMER_ABSTIME, &tspec, NULL);
        registry_read_event_fd(fd);
        return fd;
    }

    int Reactor::registry_signal_event()
    {
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGINT);
        if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1)
        {
            throw KernelException("sigprocmask() failed");
        }
        int fd = signalfd(-1, &mask, 0);
        if (fd == -1)
        {
            throw KernelException("signalfd() failed");
        }
        registry_read_event_fd(fd);
        return fd;
    }

    // START LOOP
    void Reactor::start()
    {
        if (on_timer_cb != NULL)
        {
            timer_fd = registry_timer_event(10);
        }
        if (on_read_cb == NULL)
        {
            throw KernelException("registry_callback_on_read() nust be called before start()");
        }
        struct epoll_event active_events[FASTEV_REACTOR_POLL_SIZE];
        while (active)
        {
            int found_events = epoll_wait(event_base, active_events, FASTEV_REACTOR_POLL_SIZE, -1);
            if (found_events < 0)
            {
                break;
            }
            if (found_events == 0)
            {
                throw KernelException("error code:%d on 'epoll_wait()'", found_events);
            }
            for (int i = 0; i < found_events; i++)
            {
                if (active_events[i].data.fd == sigint_fd)
                {
                    Logger::log(LogLevel::INFO, "signal SIGINT");
                    active = false;
                    break;
                }
                if (active_events[i].data.fd == timer_fd)
                {
                    char buf[8];
                    ssize_t bytes = read(timer_fd, &buf, 8);
                    on_timer_cb();
                    continue;
                }
                on_read_cb(active_events[i].data.fd);
            }
        }
    }
} // namespace fastev
#endif