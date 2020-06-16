#include "buffer.hpp"

namespace fastev
{
    void Buffer::append(char *chunk, size_t size)
    {
        if (_data.size() > FASTEV_BUFF_SIZE)
        {
            throw KernelException("buffer size %d overflow", FASTEV_BUFF_SIZE);
        }
        _data.append(chunk, size);
        last_access = chrono::system_clock::now();
        if (delimiter_pos == 0)
        {
            delimiter_pos = getDelimiter(chunk, size);
            if (delimiter_pos == 0)
            {
                return;
            }
        }
        if (content_len == 0)
        {
            content_len = getContenLen();
            if (content_len == 0)
            {
                is_full = true;
                return;
            }
        }
        if (_data.size() >= delimiter_pos + 4 + content_len)
        {
            is_full = true;
        }
    }

    int Buffer::getContenLen()
    {
        size_t pos1 = _data.find("Content-Length: ");
        if (pos1 == string::npos)
        {
            return 0;
        }
        size_t pos2 = _data.find("\r\n", pos1);
        if (pos2 == string::npos)
        {
            throw KernelException("request format does not follow protocol");
        }
        return std::stoi(_data.substr(pos1 + 16, pos2 - pos1 - 16));
    }

    int Buffer::getDelimiter(char *bytes, size_t size)
    {
        if (size > 3)
        {
            auto result = strstr(bytes, "\r\n\r\n");
            int pos = result - bytes;
            if (pos > 0)
            {
                return _data.size() - (size - (size_t)pos);
            }
            return 0;
        }
        size_t pos = _data.find("\r\n\r\n");
        if (pos > 0)
        {
            return pos;
        }
        return 0;
    }

    bool Buffer::isFull()
    {
        return is_full;
    }

    void Buffer::reset()
    {
        _data.clear();
        content_len = 0;
        delimiter_pos = 0;
        is_full = false;
        last_access = chrono::system_clock::now();
    }

    bool Buffer::older(int seconds)
    {
        return chrono::duration_cast<chrono::seconds>(chrono::system_clock::now() - last_access) > chrono::seconds(seconds);
    }

    bool Buffer::hasBody()
    {
        return content_len > 0;
    }

    string Buffer::headerStr()
    {
        return _data.substr(0, delimiter_pos);
    }

    string Buffer::bodyStr()
    {
        return _data.substr(delimiter_pos + 4;
    }
} // namespace fastev