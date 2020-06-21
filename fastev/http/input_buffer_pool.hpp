#ifndef FASTEV_INPUT_BUFFER_POOL
#define FASTEV_INPUT_BUFFER_POOL

#include <vector>
#include <iterator>
#include "input_buffer.hpp"
#include "../core/logger.hpp"
#include "../core/exception.hpp"

namespace fastev
{
    class InputBufferPool
    {
    private:
        std::vector<InputBuffer *> pool;

    public:
        ~InputBufferPool();
        InputBuffer *get();
        void put(InputBuffer *buf);
    };

} // namespace fastev

#endif