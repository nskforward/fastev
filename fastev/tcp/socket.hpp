#ifndef FASTEV_SOCKET_HPP
#define FASTEV_SOCKET_HPP

#include <netdb.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include "../core/exception.hpp"

namespace fastev
{
    int get_tcp_socket_fd(int port);
    int accept_tcp_connection(int master_sock, char *ip, int *port);
} // namespace fastev

#endif