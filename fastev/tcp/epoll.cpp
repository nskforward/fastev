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
        sigint_fd = registrySignal();
        Logger::log(LogLevel::INFO, "event method is epoll");
    }

    Reactor::~Reactor()
    {
        close(event_base);
    }

    int Reactor::registryTimer(time_t seconds)
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
        watch(fd);
        return fd;
    }

    int Reactor::registrySignal()
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
        watch(fd);
        return fd;
    }

    void Reactor::watch(int fd)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if (epoll_ctl(event_base, EPOLL_CTL_ADD, fd, &ev) == -1)
        {
            throw KernelException("cannot registry an event");
        }
    }

    void Reactor::unwatch(int fd)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if (epoll_ctl(event_base, EPOLL_CTL_DEL, fd, &ev) == -1)
        {
            throw KernelException("cannot registry an event");
        }
    }

    void Reactor::onTimer(std::function<void()> func)
    {
        timer_fd = registryTimer(10);
        timer_cb = func;
    }

    // START LOOP
    void Reactor::start()
    {
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
                    timer_cb();
                    continue;
                }
                onSocketEvent(active_events[i].data.fd);
            }
        }
    }
} // namespace fastev
#endif