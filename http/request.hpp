#ifndef FASTEV_HTTP_BYTE_BUFFER
#define FASTEV_HTTP_BYTE_BUFFER

#ifndef FASTEV_HTTP_MAX_HEADERS_SIZE
#define FASTEV_HTTP_MAX_HEADERS_SIZE 1024
#endif

#ifndef FASTEV_HTTP_MAX_BODY_SIZE
#define FASTEV_HTTP_MAX_BODY_SIZE 4096
#endif

#ifndef FASTEV_HTTP_MAX_URI_SIZE
#define FASTEV_HTTP_MAX_URI_SIZE 512
#endif

#ifndef FASTEV_HTTP_MAX_HEADER_VALUE_SIZE
#define FASTEV_HTTP_MAX_HEADER_VALUE_SIZE 512
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <unordered_map>
#include "../core/logger.hpp"
#include "../core/byte_buffer.hpp"
#include "exception.hpp"
#include "parser.hpp"
#include "utils.hpp"
#include <chrono>
#include <thread>

using namespace std;

namespace fastev
{
    class Request
    {
    private:
        int _fd;
        int _worker_id;
        char _method[8];
        char _uri[128];
        char _proto_ver[9];
        bool _needs_load = false;
        size_t _body_size = 0;
        unordered_map<string, string> _headers;
        ByteBuffer<1024> *_body;

    public:
        Request(int fd, int worker_id, ByteBuffer<1024> *buffer, size_t delimiter_pos);
        char *method();
        char *uri();
        char *proto_ver();
        int fd();
        int worker_id();

        unordered_map<string, string> &headers();
        ByteBuffer<1024> *body();
        void answer(string message);
        void answer(int code, string message);
        void answer(int code, string message, unordered_map<string, string> &headers);
        bool needsLoad();
        void load();
    };

    bool Request::needsLoad()
    {
        return _needs_load;
    }

    void Request::load()
    {
        if (_body_size > 1024)
        {
            throw KernelException("Too large body size %d bytes, allow max %d bytes", _body_size, 1024);
        }
        auto t0 = chrono::high_resolution_clock::now();
        while (_body->len() < _body_size)
        {
            char chunk[1024];
            auto ret = ::recv(_fd, chunk, 1024, 0);
            if (ret < 0)
            {
                if (errno == EAGAIN)
                {
                    auto t1 = chrono::high_resolution_clock::now();
                    auto fs = t1 - t0;
                    if (fs > std::chrono::seconds(5))
                    {
                        throw KernelException("[#%d] read body timeout 5 seconds", _fd);
                    }
                    std::this_thread::sleep_for(std::chrono::nanoseconds(1));
                    continue;
                }
                break;
            }
            if (ret == 0) // client disconnected
            {
                throw KernelException("client disconnected");
            }
            _body->append(chunk, ret);
        }
    }

    Request::Request(int fd, int worker_id, ByteBuffer<1024> *buffer, size_t delimiter_pos)
    {
        _fd = fd;
        _worker_id = worker_id;
        HTTPParser::parse(_method, _uri, _proto_ver, _headers, buffer, delimiter_pos);
        buffer->shift(delimiter_pos + 4);
        if (_headers.count("Content-Length") > 0)
        {
            _body_size = std::stoi(_headers["Content-Length"]);
            _needs_load = buffer->len() < _body_size;
        }
        _body = buffer;
    }

    void Request::answer(string message)
    {
        answer(200, message);
    }

    void Request::answer(int code, string message)
    {
        unordered_map<string, string> headers;
        headers["Content-Type"] = "text/html;charset=utf-8";
        answer(code, message, headers);
    }

    void Request::answer(int code, string message, unordered_map<string, string> &headers)
    {
        std::stringstream ss;
        ss << "HTTP/1.1 " << http_code_to_str(code) << "\r\n";
        headers["Content-Length"] = std::to_string(message.size());
        std::unordered_map<std::string, std::string>::iterator it = headers.begin();
        while (it != headers.end())
        {
            ss << it->first << ": " << it->second << "\r\n";
            it++;
        }
        ss << "\r\n"
           << message;
        auto ret = ::send(_fd, ss.str().c_str(), ss.str().size(), 0);
        if (ret < ss.str().size())
        {
            Logger::log(LogLevel::ERROR, "send %d of %d bytes", ret, ss.str().size());
        }
        Logger::log(LogLevel::INFO, "[#%d] %d %s %s %s", _fd, code, _method, _uri, _proto_ver);
    }

    char *Request::method()
    {
        return _method;
    }

    char *Request::uri()
    {
        return _uri;
    }

    char *Request::proto_ver()
    {
        return _proto_ver;
    }

    int Request::fd()
    {
        return _fd;
    }

    int Request::worker_id()
    {
        return _worker_id;
    }

    unordered_map<string, string> &Request::headers()
    {
        return _headers;
    }

    ByteBuffer<1024> *Request::body()
    {
        return _body;
    }

} // namespace fastev

#endif