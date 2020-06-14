#include "tcp_server.hpp"

namespace fastev
{
    TCPServer::TCPServer(int port)
    {
        master_sock = get_tcp_socket_fd(port);
        registry_read_event_fd(master_sock);
        registry_callback_on_read([this](int fd) {
            onRead(fd);
        });
        Logger::log(LogLevel::INFO, "server is started on port %d", port);
    }

    TCPServer::~TCPServer()
    {
        close(master_sock);
        Logger::log(LogLevel::INFO, "server is stopped");
    }

    void TCPServer::start()
    {
        if (on_data_cb == NULL)
        {
            throw KernelException("TCPServer::onData() must be called before TCPServer::start()");
        }
        Reactor::start();
    }

    void TCPServer::onRead(int fd)
    {
        // NEW CONNECT
        if (fd == master_sock)
        {
            char ip[32];
            int port;
            int child_socket = accept_tcp_connection(master_sock, ip, &port);
            if (child_socket > 0)
            {
                registry_read_event_fd(child_socket);
                if (on_connect_cb != NULL)
                {
                    on_connect_cb(child_socket, ip, port);
                }
            }
            return;
        }
        // NEW DATA
        char chunk[1024];
        size_t bytes_read = recv(fd, chunk, sizeof(chunk), 0);
        if (bytes_read < 1) // client disconnected
        {
            disconnect(fd);
            return;
        }
        on_data_cb(fd, chunk, bytes_read);
    }

    void TCPServer::tcpReply(int fd, const char *message, size_t size)
    {
        send(fd, message, size, 0);
    }

    void TCPServer::onConnect(std::function<void(int fd, char *ip, int port)> func)
    {
        on_connect_cb = func;
    }

    void TCPServer::onDisconnect(std::function<void(int fd)> func)
    {
        on_disconnect_cb = func;
    }

    void TCPServer::onData(std::function<void(int fd, char *data, size_t size)> func)
    {
        on_data_cb = func;
    }

    void TCPServer::onTimer(std::function<void()> func)
    {
        registry_callback_on_timer(func);
    }

    void TCPServer::disconnect(int fd)
    {
        unregistry_read_event_fd(fd);
        close(fd);
        if (on_disconnect_cb != NULL)
        {
            on_disconnect_cb(fd);
        }
    }
} // namespace fastev