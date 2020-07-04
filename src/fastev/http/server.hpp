#ifndef FASTEV_HTTP_SERVER
#define FASTEV_HTTP_SERVER
#include "../tcp/server.hpp"
#include "../core/byte_buffer.hpp"
#include "request.hpp"
#include <unordered_map>
#include <sstream>

#ifndef FASTEV_HTTP_MAX_CONNECTIONS
#define FASTEV_HTTP_MAX_CONNECTIONS 128
#endif

using namespace std;

namespace fastev
{
    class HTTPServer : public TCPServer
    {
    private:
        ByteBuffer<1024> buffer_pool[FASTEV_HTTP_MAX_CONNECTIONS + 1];
        function<void(Request &req)> on_request_cb;
        function<void(int fd, int worker_id)> on_disconnect_cb;
        void onData(int fd, int worker_id, const char *chunk, size_t size);

    public:
        HTTPServer(int port);
        void onRequest(function<void(Request &req)> func);
        void onDisconnect(function<void(int fd, int worker_id)> func);
    };

    HTTPServer::HTTPServer(int port) : TCPServer(port)
    {
        TCPServer::onChunk([this](int fd, int worker_id, const char *chunk, size_t size) {
            HTTPServer::onData(fd, worker_id, chunk, size);
        });
        TCPServer::onDisonnect([this](int fd, int worker_id) {
            auto buff = &buffer_pool[fd];
            buff->reset();
            on_disconnect_cb(fd, worker_id);
        });
    }

    void HTTPServer::onRequest(function<void(Request &req)> func)
    {
        on_request_cb = func;
    }

    void HTTPServer::onData(int fd, int worker_id, const char *chunk, size_t size)
    {
        if (fd > FASTEV_HTTP_MAX_CONNECTIONS)
        {
            throw KernelException("max number of connections (%d) is exeeded", FASTEV_HTTP_MAX_CONNECTIONS);
        }
        auto buff = &buffer_pool[fd];
        buff->append(chunk, size);
        auto pos = buff->str_pos("\r\n\r\n");
        if (pos > 0)
        {
            auto req = Request(fd, worker_id, buff, pos);
            try
            {
                on_request_cb(req);
            }
            catch (const std::exception &e)
            {
                req.answer(500, "Internal Server Error");
                Logger::log(LogLevel::ERROR, e.what());
            }
            buff->reset();
        }
    }
} // namespace fastev

#endif