#include "server.hpp"

namespace fastev
{
    TCPServer::TCPServer(int port) : Reactor()
    {
        master_sock = get_tcp_socket_fd(port);
        watch(master_sock);
        Logger::log(LogLevel::INFO, "server is started on port %d", port);
    }

    TCPServer::~TCPServer()
    {
        close(master_sock);
        Logger::log(LogLevel::INFO, "server is stopped");
    }

    void TCPServer::disconnect(int fd)
    {
        unwatch(fd);
        close(fd);
        if (disconnect_cb != NULL)
        {
            disconnect_cb(fd);
        }
    }

    void TCPServer::tcpReply(int fd, const char *message, size_t size)
    {
        send(fd, message, size, 0);
    }

    void TCPServer::onConnect(TCPConnectFunc func)
    {
        connect_cb = func;
    }

    void TCPServer::onDisconnect(TCPDisconnectFunc func)
    {
        disconnect_cb = func;
    }

    void TCPServer::onChunk(TCPChunkFunc func)
    {
        chunk_cb = func;
    }

    void TCPServer::onSocketEvent(int fd)
    {
        if (fd == master_sock) // new connect
        {
            char ip[32];
            int port;
            int child_socket = accept_tcp_connection(master_sock, ip, &port);
            if (child_socket > 0)
            {
                watch(child_socket);
                if (connect_cb != NULL)
                {
                    connect_cb(child_socket, ip, port);
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
        if (bytes_read > 1024)
        {
            return;
        }
        chunk_cb(fd, chunk, bytes_read);
    }
} // namespace fastev