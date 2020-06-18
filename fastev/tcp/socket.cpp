#include "socket.hpp"

namespace fastev
{
    int get_tcp_socket_fd(int port)
    {
        int local_s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (local_s == -1)
        {
            throw KernelException("cannot create socket fd");
        }
        int optval = 1;
        if (setsockopt(local_s, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
        {
            close(local_s);
            throw KernelException("cannot setsockopt SOL_SOCKET SO_REUSEPORT");
        }
        if (setsockopt(local_s, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)) == -1)
        {
            close(local_s);
            throw KernelException("cannot setsockopt SOL_SOCKET SO_KEEPALIVE");
        }
        fcntl(local_s, F_SETFL, O_NONBLOCK);
        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(struct sockaddr_in));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        if (bind(local_s, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        {
            close(local_s);
            throw KernelException("cannot bind socket to port");
        }
        if (listen(local_s, SOMAXCONN) != 0)
        {
            close(local_s);
            throw KernelException("cannot listen socket");
        }
        return local_s;
    }

    int accept_tcp_connection(int master_sock, char *ip, int *port)
    {
        struct sockaddr addr;
        socklen_t socklen = sizeof(addr);
        int child_sock = accept(master_sock, &addr, &socklen);
        if (child_sock == -1)
        {
            throw KernelException("cannot accept connection");
        }
        fcntl(child_sock, F_SETFL, O_NONBLOCK);
        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
        if (getnameinfo(&addr, socklen, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV) != 0)
        {
            close(child_sock);
            throw KernelException("cannot get client address");
        }
        strcpy(ip, hbuf);
        *port = atoi(sbuf);
        return child_sock;
    }
} // namespace fastev