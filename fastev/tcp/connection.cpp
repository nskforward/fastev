#include "connection.hpp"

namespace fastev
{
    void TCPConnection::nonblocking(int fd)
    {
        int opts = fcntl(fd, F_GETFL);
        if (opts < 0)
        {
            throw KernelException("cannot get socket options");
        }
        opts = opts | O_NONBLOCK;
        if (fcntl(fd, F_SETFL, opts) < 0)
        {
            throw KernelException("cannot set socket option");
        }
    }

    int TCPConnection::listen(int port)
    {
        int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (fd == -1)
        {
            throw KernelException("cannot create socket fd");
        }
        int optval = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
        {
            throw KernelException("cannot setsockopt SOL_SOCKET SO_REUSEPORT");
        }
        if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) == -1)
        {
            throw KernelException("cannot setsockopt SOL_SOCKET SO_KEEPALIVE");
        }
        if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)) == -1)
        {
            throw KernelException("cannot setsockopt IPPROTO_TCP TCP_NODELAY");
        }
        struct sockaddr_in listenAddr;
        memset(&listenAddr, 0, sizeof(struct sockaddr_in));
        listenAddr.sin_family = AF_INET;
        listenAddr.sin_port = htons(port);
        listenAddr.sin_addr.s_addr = INADDR_ANY;
        if (bind(fd, (struct sockaddr *)&listenAddr, sizeof(listenAddr)) == -1)
        {
            throw KernelException("cannot bind socket to port");
        }
        if (::listen(fd, SOMAXCONN) != 0)
        {
            throw KernelException("cannot listen socket");
        }
        return fd;
    }

    int TCPConnection::accept(int parent_fd, struct sockaddr &addr)
    {
        socklen_t addr_size = sizeof(addr);
        int fd = ::accept(parent_fd, &addr, &addr_size);
        if (fd == -1)
        {
            throw KernelException("acceptance of new tcp connection is failed");
        }
        return fd;
    }
} // namespace fastev