#ifndef FASTEV_HTTP_SERVER
#define FASTEV_HTTP_SERVER

#include "../tcp/tcp_server.hpp"
#include "buffer_pool.hpp"
#include "http_request.hpp"
#include "http_response.hpp"
#include <map>
#include <sstream>

#ifndef FASTEV_HTTP_READ_TIMEOUT
#define FASTEV_HTTP_READ_TIMEOUT 10 // seconds
#endif

namespace fastev
{
    class HTTPServer : public TCPServer
    {
    private:
        std::function<void(int fd, HTTPRequest &req, HTTPResponse &resp)> on_data_cb;
        BufferPool buffer_pool;
        std::map<int, Buffer *> buffer_map;

        void onTCPData(int fd, char *data, size_t size);
        void onTimer();
        void onReadTimeout(int fd);
        void onHeaders(int fd, std::string headers);

    public:
        HTTPServer(int port);
        void onData(std::function<void(int fd, HTTPRequest &req, HTTPResponse &resp)> func);
        void start();
    };
} // namespace fastev

#endif