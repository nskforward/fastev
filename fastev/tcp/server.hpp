#ifndef FASTEV_TCP_SERVER
#define FASTEV_TCP_SERVER

#include "../event/kqueue.hpp"
#include "connection.hpp"

using namespace std;

namespace fastev
{
    class TCPServer : Reactor
    {
    private:
        int _listener;
        Reactor _loop;
        function<void(int fd, struct sockaddr &addr)> on_connect_cb;
        function<void(int fd)> on_disconnect_cb;

    public:
        TCPServer(int port);
        void start(function<void(int fd, char *chunk, size_t size)> callback);
        void onConnect(function<void(int fd, struct sockaddr &addr)> func);
        void onDisonnect(function<void(int fd)> func);
    };

    void TCPServer::onConnect(function<void(int fd, struct sockaddr &addr)> func)
    {
        on_connect_cb = func;
    }

    void TCPServer::onDisonnect(function<void(int fd)> func)
    {
        on_disconnect_cb = func;
    }

    TCPServer::TCPServer(int port)
    {
        _listener = TCPConnection::listen(port);
        _loop.watch(_listener);
        Logger::log(LogLevel::INFO, "server is listening on port %d", port);
    }

    void TCPServer::start(function<void(int fd, char *chunk, size_t size)> callback)
    {
        _loop.start([&](int fd) {
            if (fd == _listener) // connect
            {
                struct sockaddr addr;
                int child_fd = TCPConnection::accept(_listener, addr);
                _loop.watch(child_fd);
                if (on_connect_cb != NULL)
                {
                    on_connect_cb(child_fd, addr);
                }
                return;
            }
            char chunk[512];
            ssize_t ret = recv(fd, chunk, 512, 0);
            if (ret < 1 || ret > 512) // disconnect
            {
                _loop.unwatch(fd);
                close(fd);
                if (on_disconnect_cb != NULL)
                {
                    on_disconnect_cb(fd);
                }
                return;
            }
            callback(fd, chunk, ret);
        });
    }
} // namespace fastev

#endif