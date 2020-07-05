#ifndef FASTEV_TCP_CONNECTION
#define FASTEV_TCP_CONNECTION

#include <netdb.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include "socket.hpp"
#include "../core/exception.hpp"
#include "../core/byte_buffer.hpp"

namespace fastev
{
    class TCPConnection
    {
    private:
    public:
        ByteBuffer<1024> input;
        ByteBuffer<1024> output;
        TCPSocket *socket;
        struct sockaddr addr = {0};
    };

} // namespace fastev

#endif