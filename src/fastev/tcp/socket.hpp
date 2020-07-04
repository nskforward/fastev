#ifndef FASTEV_TCP_SOCKET
#define FASTEV_TCP_SOCKET

#include <netinet/tcp.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include "../core/exception.hpp"

namespace fastev
{
    class TCPSocket
    {
    public:
        static void reuseport(int fd);
        static void keepalive(int fd);
        static void nodelay(int fd);
        static void nonblocking(int fd);
        static void bind(int fd, int port);
        static void listen(int fd);
        static int accept(int listener, struct sockaddr &addr);
        static int create();
        static char *addrToStr(struct sockaddr &addr, char *ip);
    };

    int TCPSocket::create()
    {
        int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (fd == -1)
        {
            throw KernelException("cannot create socket fd");
        }
        return fd;
    }

    void TCPSocket::listen(int fd)
    {
        if (::listen(fd, SOMAXCONN) != 0)
        {
            throw KernelException("cannot listen socket");
        }
    }

    void TCPSocket::bind(int fd, int port)
    {
        struct sockaddr_in listenAddr;
        memset(&listenAddr, 0, sizeof(struct sockaddr_in));
        listenAddr.sin_family = AF_INET;
        listenAddr.sin_port = htons(port);
        listenAddr.sin_addr.s_addr = INADDR_ANY;
        if (::bind(fd, (struct sockaddr *)&listenAddr, sizeof(listenAddr)) == -1)
        {
            throw KernelException("cannot bind socket to port");
        }
    }

    void TCPSocket::nonblocking(int fd)
    {
        int opts = fcntl(fd, F_GETFL);
        if (opts < 0)
        {
            throw KernelException("cannot get socket options");
        }
        opts = opts | O_NONBLOCK;
        if (fcntl(fd, F_SETFL, opts) < 0)
        {
            throw KernelException("cannot set socket options");
        }
    }

    void TCPSocket::nodelay(int fd)
    {
        int optval = 1;
        if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval)) == -1)
        {
            throw KernelException("cannot setsockopt IPPROTO_TCP TCP_NODELAY");
        }
    }

    void TCPSocket::keepalive(int fd)
    {
        int optval = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) == -1)
        {
            throw KernelException("cannot setsockopt SOL_SOCKET SO_KEEPALIVE");
        }
    }

    void TCPSocket::reuseport(int fd)
    {
        int optval = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
        {
            throw KernelException("cannot setsockopt SOL_SOCKET SO_REUSEPORT");
        }
    }

    int TCPSocket::accept(int listener, struct sockaddr &addr)
    {
        socklen_t addr_size = sizeof(addr);
        return ::accept(listener, &addr, &addr_size);
    }

    char *TCPSocket::addrToStr(struct sockaddr &addr, char *ip)
    {
        inet_ntop(AF_INET, &(((struct sockaddr_in *)&addr)->sin_addr), ip, sizeof(addr));
        return ip;
    }

} // namespace fastev

#endif