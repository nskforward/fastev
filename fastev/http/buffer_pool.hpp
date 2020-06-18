#ifndef FASTEV_BUFFER_POOL_HPP
#define FASTEV_BUFFER_POOL_HPP

#include <vector>
#include <iterator>
#include "byte_buffer.hpp"
#include "../core/logger.hpp"
#include "../core/exception.hpp"

namespace fastev
{
    class BufferPool
    {
    private:
        std::vector<ByteBuffer *> pool;

    public:
        ~BufferPool();
        ByteBuffer *get();
        void put(ByteBuffer *buf);
    };

} // namespace fastev

#endif