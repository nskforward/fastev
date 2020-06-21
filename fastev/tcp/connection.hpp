#ifndef FASTEV_TCP_CONNECTION
#define FASTEV_TCP_CONNECTION

#include <netdb.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include "../core/exception.hpp"

namespace fastev
{
    class TCPConnection
    {
    public:
        static int accept(int fd, struct sockaddr &addr);
        static int listen(int port);
        static void nonblocking(int fd);
    };

} // namespace fastev

#endif