#include "buffer_pool.hpp"

namespace fastev
{
    BufferPool::BufferPool()
    {
        for (size_t i = 0; i < FASTEV_BUFF_POOL_SIZE; i++)
        {
            pool.push_back(new Buffer());
        }
    }

    BufferPool::~BufferPool()
    {
        for (size_t i = 0; i < pool.size(); i++)
        {
            delete pool[i];
        }
    }

    Buffer *BufferPool::get()
    {
        if (pool.size() == 0)
        {
            throw KernelException("pool size is empty");
        }
        Buffer *buf = pool.back();
        buf->reset();
        pool.pop_back();
        return buf;
    }

    void BufferPool::put(Buffer *buf)
    {
        pool.push_back(buf);
    }
} // namespace fastev