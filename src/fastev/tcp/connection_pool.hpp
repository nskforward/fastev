#ifndef FASTEV_TCP_CONNECTION_POOL
#define FASTEV_TCP_CONNECTION_POOL

#include "connection.hpp"
#include "../core/logger.hpp"

namespace fastev
{
    class ConnectionPool
    {
    private:
        TCPConnection pool[10];

    public:
        ~ConnectionPool();
        TCPConnection &get(int fd);
    };

} // namespace fastev

#endif