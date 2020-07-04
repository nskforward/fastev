#ifndef FASTEV_BYTE_BUFFER_HPP
#define FASTEV_BYTE_BUFFER_HPP

#include <iostream>
#include "exception.hpp"
#include "logger.hpp"

namespace fastev
{
    template <size_t SIZE>
    class ByteBuffer
    {
    private:
        char _data[SIZE];
        size_t _pos = 0;

    public:
        size_t len();
        size_t cap();
        void reset();
        void append(std::string src);
        void append(const char *src, size_t size);
        char *c_str();
        ssize_t str_pos(const char *search);
        ssize_t str_pos(std::string search);
        void shift(size_t index);
    };

    template <size_t SIZE>
    size_t ByteBuffer<SIZE>::len()
    {
        return _pos;
    }

    template <size_t SIZE>
    size_t ByteBuffer<SIZE>::cap()
    {
        return SIZE;
    }

    template <size_t SIZE>
    void ByteBuffer<SIZE>::reset()
    {
        _pos = 0;
    }

    template <size_t SIZE>
    void ByteBuffer<SIZE>::append(std::string src)
    {
        append(src.c_str(), src.size());
    }

    template <size_t SIZE>
    void ByteBuffer<SIZE>::append(const char *src, size_t size)
    {
        if (_pos + size > SIZE)
        {
            throw KernelException("buffer size %d overflows by %d", SIZE, _pos + size);
        }
        memcpy(_data + _pos, src, size);
        _pos += size;
        _data[_pos] = '\0';
    }

    template <size_t SIZE>
    char *ByteBuffer<SIZE>::c_str()
    {
        _data[_pos] = '\0';
        return _data;
    }

    template <size_t SIZE>
    ssize_t ByteBuffer<SIZE>::str_pos(const char *search)
    {
        if (_pos < 4)
        {
            return -1;
        }
        auto found = strstr(_data, search);
        if (found == NULL)
        {
            return -1;
        }
        return found - _data;
    }

    template <size_t SIZE>
    ssize_t ByteBuffer<SIZE>::str_pos(std::string search)
    {
        return str_pos(search.c_str());
    }

    template <size_t SIZE>
    void ByteBuffer<SIZE>::shift(size_t index)
    {
        if (index < _pos)
        {
            size_t new_len = _pos - index;
            memcpy(_data, _data + index, new_len);
            _pos = new_len;
        }
    }

} // namespace fastev

#endif