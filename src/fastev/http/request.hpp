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
    };

    Request::Request(int fd, int worker_id, ByteBuffer<1024> *buffer, size_t delimiter_pos)
    {
        _fd = fd;
        _worker_id = worker_id;
        HTTPParser::parse(_method, _uri, _proto_ver, _headers, buffer, delimiter_pos);
        buffer->shift(delimiter_pos + 4);
        _body = buffer;
    }

    void Request::answer(string message)
    {
        std::stringstream ss;
        unordered_map<string, string> headers;
        headers["Content-Type"] = "text/html;charset=UTF-8";
        headers["Content-Length"] = std::to_string(message.size());

        ss << "HTTP/1.1 200 OK\r\n";
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
            throw KernelException("partial send");
        }
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