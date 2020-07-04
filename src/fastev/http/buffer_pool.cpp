#include "buffer_pool.hpp"

namespace fastev
{
    HTTPBuffer &HTTPBufferPool::getBuffer(int fd)
    {
        if (fd > 511)
        {
            throw KernelException("buffer pool overflow");
        }
        return pool[fd];
    }

} // namespace fastev