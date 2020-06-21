#ifndef FASTEV_CONNECTION_POOL
#define FASTEV_CONNECTION_POOL

#include <vector>
#include <iterator>
#include <mutex>
#include "connection.hpp"
#include "../core/logger.hpp"

namespace fastev
{
    class ConnectionPool
    {
    private:
        std::vector<TCPConnection *> pool;
        std::mutex mtx;

    public:
        ~ConnectionPool();
        TCPConnection *get();
        void put(TCPConnection *conn);
    };

} // namespace fastev

#endif