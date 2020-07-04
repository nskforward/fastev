#ifndef FASTEV_TCP_SERVER
#define FASTEV_TCP_SERVER

#include "../event/kqueue.hpp"
#include "thread_pool.hpp"
#include "socket.hpp"

using namespace std;

namespace fastev
{
    class TCPServer : public Reactor
    {
    private:
        int _listener;
        ThreadPool pool;
        function<void(int fd, struct sockaddr &addr)> on_connect_cb;
        void onRead(int fd);
        void canWrite(int fd);

    public:
        TCPServer(int port);
        void onConnect(function<void(int fd, struct sockaddr &addr)> func);
        void onDisonnect(function<void(int fd, int worker_id)> func);
        void onChunk(function<void(int fd, int worker_id, const char *chunk, size_t size)> func);
    };

    TCPServer::TCPServer(int port) : Reactor()
    {
        _listener = TCPSocket::create();
        TCPSocket::keepalive(_listener);
        TCPSocket::reuseport(_listener);
        TCPSocket::nonblocking(_listener);
        TCPSocket::bind(_listener, port);
        TCPSocket::listen(_listener);
        Reactor::onRead([this](int fd) {
            TCPServer::onRead(fd);
        });
        Reactor::watchRead(_listener);
        Logger::log(LogLevel::INFO, "server is listening on port %d", port);
    }

    void TCPServer::onConnect(function<void(int fd, struct sockaddr &addr)> func)
    {
        on_connect_cb = func;
    }

    void TCPServer::onDisonnect(function<void(int fd, int worker_id)> func)
    {
        pool.onDisonnect(func);
    }

    void TCPServer::onChunk(function<void(int fd, int worker_id, const char *chunk, size_t size)> func)
    {
        pool.onChunk(func);
    }

    void TCPServer::onRead(int fd)
    {
        if (fd == _listener) // connect
        {
            struct sockaddr addr;
            int new_fd = TCPSocket::accept(_listener, addr);
            if (new_fd < 0)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    Logger::log(LogLevel::INFO, "accept is blocked");
                }
                else
                {
                    Logger::log(LogLevel::ERROR, "accept returned error #%d", errno);
                }
                return;
            }
            TCPSocket::nonblocking(new_fd);
            pool.watch(new_fd);
            if (on_connect_cb != NULL)
            {
                on_connect_cb(new_fd, addr);
            };
        }
    }
} // namespace fastev

#endif