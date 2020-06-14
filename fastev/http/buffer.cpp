#include "buffer.hpp"

namespace fastev
{
    void Buffer::append(char *bytes, size_t size)
    {
        if (pos + size > FASTEV_BUFF_SIZE)
        {
            throw KernelException("buffer size %d overflow", FASTEV_BUFF_SIZE);
        }
        std::memcpy(&data[pos], bytes, size);
        pos += size;
        last_access = std::chrono::system_clock::now();
    }

    size_t Buffer::length()
    {
        return pos;
    }

    void Buffer::reset()
    {
        pos = 0;
        last_access = std::chrono::system_clock::now();
    }

    bool Buffer::older(int seconds)
    {
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now() - last_access) > std::chrono::seconds(seconds);
    }

    std::string Buffer::str()
    {
        return std::string(data, pos);
    }
} // namespace fastev