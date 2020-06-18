#ifndef FASTEV_HTTP_SERVER
#define FASTEV_HTTP_SERVER
#include "../tcp/server.hpp"
#include "buffer_pool.hpp"
#include <iterator>
#include "utils.hpp"
#include "response.hpp"
#include "exception.hpp"
#include <sstream>

using namespace std;

namespace fastev
{
    class HTTPServer : public TCPServer
    {
    private:
        function<void(int fd, ByteBuffer *buf)> request_cb;
        map<int, ByteBuffer *> buffer_map;
        BufferPool buffer_pool;
        stringstream reply_buf;

        void onTimer();
        void onChunk(int fd, char *data, size_t size);
        void onBufferFull(int fd, ByteBuffer *data);

    public:
        HTTPServer(int port);
        ~HTTPServer();
        void onRequest(function<void(int fd, ByteBuffer *buf)> func);
        void httpReply(int fd, HTTPCode code, string type, string body);
    };

} // namespace fastev

#endif