#include "epoll.hpp"

#ifdef __linux__
namespace fastev
{
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
        watchRead(fd);
        return fd;
    }

    Reactor::Reactor()
    {
        _event_base = epoll_create1(0);
        if (_event_base == -1)
        {
            throw KernelException("cannot create event base epoll");
        }
        _sigint_fd = registrySignal();
        //Logger::log(LogLevel::INFO, "event method is epoll");
    }

    Reactor::~Reactor()
    {
        close(_event_base);
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
        watchRead(fd);
        return fd;
    }

    void Reactor::watchRead(int fd)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if (epoll_ctl(_event_base, EPOLL_CTL_ADD, fd, &ev) == -1)
        {
            throw KernelException("cannot registry an event");
        }
    }

    void Reactor::unwatchRead(int fd)
    {
        struct epoll_event ev;
        ev.events = EPOLLIN;
        ev.data.fd = fd;
        if (epoll_ctl(_event_base, EPOLL_CTL_DEL, fd, &ev) == -1)
        {
            throw KernelException("cannot registry an event");
        }
    }

    void Reactor::onTimer(std::function<void()> func)
    {
        _timer_fd = registryTimer(10);
        timer_cb = func;
    }

    void Reactor::onRead(function<void(int fd)> func)
    {
        read_cb = func;
    }

    // START LOOP
    void Reactor::start()
    {
        struct epoll_event active_events[FASTEV_REACTOR_POLL_SIZE];
        while (_active)
        {
            int found_events = epoll_wait(_event_base, active_events, FASTEV_REACTOR_POLL_SIZE, 2000);
            if (found_events < 0)
            {
                throw KernelException("epoll_wait error #%d", errno);
            }
            if (found_events == 0) // timeout
            {
                continue;
            }
            for (int i = 0; i < found_events; i++)
            {
                if (active_events[i].data.fd == _sigint_fd)
                {
                    //Logger::log(LogLevel::INFO, "signal SIGINT");
                    _active = false;
                    break;
                }
                if (active_events[i].data.fd == _timer_fd)
                {
                    char buf[8];
                    ssize_t bytes = read(_timer_fd, &buf, 8);
                    timer_cb();
                    continue;
                }
                read_cb(active_events[i].data.fd);
            }
        }
    }
} // namespace fastev
#endif