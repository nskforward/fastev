#ifndef FASTEV_TCP_SERVER
#define FASTEV_TCP_SERVER

#include "../event/kqueue.hpp"
#include "connection.hpp"

#ifndef FASTEV_TCP_CHUNK_SIZE
#define FASTEV_TCP_CHUNK_SIZE 512
#endif

using namespace std;

namespace fastev
{
    class TCPServer : public Reactor
    {
    private:
        int _listener;
        Reactor _loop;
        function<void(int fd, struct sockaddr &addr)> on_connect_cb;
        function<void(int fd)> on_disconnect_cb;

    public:
        TCPServer(int port);
        void start(function<void *()> get_buffer, function<void(void *buff)> put_buffer, function<void(int fd, void *buff, char *chunk, size_t size)> callback);
        void onConnect(function<void(int fd, struct sockaddr &addr)> func);
        void onDisonnect(function<void(int fd)> func);
        void tcpReply(int fd, string data);
    };

    void TCPServer::tcpReply(int fd, string data)
    {
        ssize_t ret = send(fd, data.c_str(), data.size(), 0);
        if (ret != data.size())
        {
            throw KernelException("sent less bytes than expected");
        }
    }

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

    void TCPServer::start(
        function<void *()> get_buffer,
        function<void(void *buff)> put_buffer,
        function<void(int fd, void *buff, char *chunk, size_t size)> callback)
    {
        _loop.start([&](int fd, void *buff) {
            if (fd == _listener) // connect
            {
                struct sockaddr addr;
                int child_fd = TCPConnection::accept(_listener, addr);
                _loop.watch(child_fd, get_buffer());
                if (on_connect_cb != NULL)
                {
                    on_connect_cb(child_fd, addr);
                }
                return;
            }
            char chunk[FASTEV_TCP_CHUNK_SIZE];
            ssize_t ret = recv(fd, chunk, FASTEV_TCP_CHUNK_SIZE, 0);
            if (ret < 1 || ret > FASTEV_TCP_CHUNK_SIZE) // disconnect
            {
                _loop.unwatch(fd);
                close(fd);
                put_buffer(buff);
                if (on_disconnect_cb != NULL)
                {
                    on_disconnect_cb(fd);
                }
                return;
            }
            callback(fd, buff, chunk, ret);
        });
    }
} // namespace fastev

#endif