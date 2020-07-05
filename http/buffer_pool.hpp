#ifndef FASTEV_TCP_BUFFER_POOL
#define FASTEV_TCP_BUFFER_POOL

#include "../core/byte_buffer.hpp"

namespace fastev
{
    struct HTTPBuffer
    {
        ByteBuffer<1024> input_buffer;
        ByteBuffer<1> output_buffer;
    };

    class HTTPBufferPool
    {
    private:
        HTTPBuffer pool[512];

    public:
        HTTPBuffer &getBuffer(int fd);
    };
} // namespace fastev

#endif