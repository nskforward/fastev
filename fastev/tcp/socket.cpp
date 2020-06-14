#include "socket.hpp"

namespace fastev
{
    int get_tcp_socket_fd(int port)
    {
        struct addrinfo *addr;
        struct addrinfo hints;

        /* open a TCP socket */
        memset(&hints, 0, sizeof hints);
        hints.ai_family = PF_UNSPEC;     /* any supported protocol */
        hints.ai_flags = AI_PASSIVE;     /* result for bind() */
        hints.ai_socktype = SOCK_STREAM; /* TCP */
        char port_str[5];
        int error = getaddrinfo("0.0.0.0", std::to_string(port).c_str(), &hints, &addr);
        if (error)
        {
            throw KernelException("cannot get addrinfo: %s", gai_strerror(error));
        }

        int local_s = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (local_s == -1)
        {
            std::cout << "cannot create socket" << std::endl;
            throw KernelException("cannot create socket fd");
        }

        // SET OPTTIONS
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

        // BIND
        if (bind(local_s, addr->ai_addr, addr->ai_addrlen) == -1)
        {
            close(local_s);
            throw KernelException("cannot bind socket to port");
        }
        if (listen(local_s, SOMAXCONN) == -1)
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