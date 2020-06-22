#include "input_buffer_pool.hpp"

namespace fastev
{
    InputBufferPool::~InputBufferPool()
    {
        for (size_t i = 0; i < pool.size(); i++)
        {
            delete pool[i];
        }
        Logger::log(LogLevel::TRACE, "deleted %d buffers", pool.size());
    }

    InputBufferPool::InputBufferPool(int size)
    {
        for (size_t i = 0; i < size; i++)
        {
            pool.push_back(new InputBuffer());
        }
    }

    InputBuffer *InputBufferPool::get()
    {
        if (pool.size() == 0)
        {
            Logger::log(LogLevel::TRACE, "++ buffer");
            return new InputBuffer();
        }
        InputBuffer *buf = pool.back();
        buf->reset();
        pool.pop_back();
        return buf;
    }

    void InputBufferPool::put(InputBuffer *buf)
    {
        pool.push_back(buf);
        //Logger::log(LogLevel::INFO, "pool size %d", pool.size());
    }
} // namespace fastev