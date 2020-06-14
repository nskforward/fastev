#ifndef FASTEV_BUFFER_HPP
#define FASTEV_BUFFER_HPP

#include <string>
#include <chrono>
#include "../core/exception.hpp"

#ifndef FASTEV_BUFF_SIZE
#define FASTEV_BUFF_SIZE 4096
#endif

namespace fastev
{
    class Buffer
    {
    private:
        char data[FASTEV_BUFF_SIZE];
        size_t pos = 0;
        std::chrono::system_clock::time_point last_access;

    public:
        void append(char *bytes, size_t size);
        size_t length();
        void reset();
        std::string str();
        bool older(int seconds);
    };
} // namespace fastev

#endif