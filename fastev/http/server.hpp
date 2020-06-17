#ifndef FASTEV_HTTP_SERVER
#define FASTEV_HTTP_SERVER
#include "../tcp/server.hpp"
#include "buffer_pool.hpp"
#include <iterator>
#include "http.hpp"
#include "response.hpp"

using namespace std;

namespace fastev
{
    class HTTPServer : public TCPServer
    {
    private:
        function<void(HTTPResponse &resp)> request_cb;
        map<int, Buffer *> buffer_map;
        BufferPool buffer_pool;

        void onTimer();
        void onChunk(int fd, char *data, size_t size);
        void onBufferFull(int fd, Buffer *data);

    public:
        HTTPServer(int port);
        ~HTTPServer();
        void onRequest(function<void(HTTPResponse &resp)> func);
    };

} // namespace fastev

#endif