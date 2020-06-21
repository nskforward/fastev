#include "connection_pool.hpp"

namespace fastev
{
    ConnectionPool::~ConnectionPool()
    {
        for (size_t i = 0; i < pool.size(); i++)
        {
            delete pool[i];
        }
        Logger::log(LogLevel::TRACE, "deleted %d connections", pool.size());
    }

    TCPConnection *ConnectionPool::get()
    {
        mtx.lock();
        if (pool.size() == 0)
        {
            Logger::log(LogLevel::TRACE, "++ connection");
            return new TCPConnection();
        }
        TCPConnection *conn = pool.back();
        pool.pop_back();
        mtx.unlock();
        return conn;
    }

    void ConnectionPool::put(TCPConnection *conn)
    {
        mtx.lock();
        pool.push_back(conn);
        mtx.unlock();
    }
} // namespace fastev