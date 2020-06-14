#ifndef FASTEV_TCP_SERVER_HPP
#define FASTEV_TCP_SERVER_HPP

#ifdef __APPLE__
#include "kqueue.hpp"
#endif
#ifdef __linux__
#include "epoll.hpp"
#endif
#include "socket.hpp"

namespace fastev
{
    class TCPServer : private Reactor
    {
    private:
        int master_sock = 0;

        std::function<void(int fd, char *ip, int port)> on_connect_cb;
        std::function<void(int fd)> on_disconnect_cb;
        std::function<void(int fd, char *data, size_t size)> on_data_cb;
        void onRead(int fd);

    public:
        TCPServer(int port);
        ~TCPServer();
        void onConnect(std::function<void(int fd, char *ip, int port)> func);
        void onDisconnect(std::function<void(int fd)> func);
        void onData(std::function<void(int fd, char *data, size_t size)> func);
        void onTimer(std::function<void()> func);
        void disconnect(int fd);

        void start();
        void tcpReply(int fd, const char *message, size_t size);
    };
} // namespace fastev

#endif