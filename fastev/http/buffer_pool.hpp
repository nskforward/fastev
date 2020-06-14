#ifndef FASTEV_BUFFER_POOL_HPP
#define FASTEV_BUFFER_POOL_HPP

#include <vector>
#include <iterator>
#include "buffer.hpp"
#include "../core/exception.hpp"

#ifndef FASTEV_BUFF_POOL_SIZE
#define FASTEV_BUFF_POOL_SIZE 32
#endif

namespace fastev
{
    class BufferPool
    {
    private:
        std::vector<Buffer *> pool;

    public:
        BufferPool();
        ~BufferPool();
        Buffer *get();
        void put(Buffer *buf);
    };
} // namespace fastev

#endif