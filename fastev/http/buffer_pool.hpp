#ifndef FASTEV_BUFFER_POOL_HPP
#define FASTEV_BUFFER_POOL_HPP

#include <vector>
#include <iterator>
#include "buffer.hpp"
#include "../core/logger.hpp"
#include "../core/exception.hpp"

namespace fastev
{
    class BufferPool
    {
    private:
        std::vector<Buffer *> pool;

    public:
        ~BufferPool();
        Buffer *get();
        void put(Buffer *buf);
    };
} // namespace fastev

#endif