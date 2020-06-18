#include "buffer_pool.hpp"

namespace fastev
{
    BufferPool::~BufferPool()
    {
        for (size_t i = 0; i < pool.size(); i++)
        {
            delete pool[i];
        }
        Logger::log(LogLevel::TRACE, "deleted %d buffers", pool.size());
    }

    ByteBuffer *BufferPool::get()
    {
        if (pool.size() == 0)
        {
            Logger::log(LogLevel::TRACE, "byte buffer allocation");
            return new ByteBuffer();
        }
        ByteBuffer *buf = pool.back();
        buf->reset();
        pool.pop_back();
        return buf;
    }

    void BufferPool::put(ByteBuffer *buf)
    {
        pool.push_back(buf);
    }
} // namespace fastev