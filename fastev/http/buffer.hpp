#ifndef FASTEV_BUFFER_HPP
#define FASTEV_BUFFER_HPP

#include <string>
#include <chrono>
#include "../core/exception.hpp"
#include "../core/logger.hpp"

#ifndef FASTEV_BUFF_SIZE
#define FASTEV_BUFF_SIZE 8192
#endif

using namespace std;

namespace fastev
{
    class Buffer
    {
    private:
        string _data;
        size_t content_len = 0;
        size_t delimiter_pos = 0;
        bool is_full = false;

        chrono::system_clock::time_point last_access;
        int getDelimiter(char *bytes, size_t size);
        int getContenLen();

    public:
        void append(char *bytes, size_t size);
        void reset();
        bool isFull();
        bool hasBody();
        string headerStr();
        string bodyStr();
        bool older(int seconds);
    };
} // namespace fastev

#endif