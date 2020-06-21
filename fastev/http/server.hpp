#ifndef FASTEV_HTTP_SERVER
#define FASTEV_HTTP_SERVER
#include "../tcp/server.hpp"
#include "input_buffer_pool.hpp"
#include "output_buffer.hpp"
#include <map>

using namespace std;

namespace fastev
{
    class HTTPServer : public TCPServer
    {
    private:
        map<int, InputBuffer *> input_buffer_map;
        function<void(InputBuffer *input_buffer, OutputBuffer *output_buffer)> on_request_cb;

    public:
        HTTPServer(int port);
        ~HTTPServer();
        void start(function<void(InputBuffer *input_buffer, OutputBuffer *output_buffer)> callback);
    };

    HTTPServer::HTTPServer(int port) : TCPServer(port) {}

    void HTTPServer::start(function<void(InputBuffer *input_buffer, OutputBuffer *output_buffer)> callback)
    {
        TCPServer::start([this, &callback](int fd, char *chunk, size_t size) {
            if (input_buffer_map.count(fd) == 0)
            {
                input_buffer_map[fd] = new InputBuffer();
            }
            InputBuffer *input_buffer = input_buffer_map[fd];
            input_buffer->append(chunk, size);
            if (input_buffer->isFull())
            {
                OutputBuffer output_buffer = OutputBuffer(input_buffer->getHost());
                callback(input_buffer, &output_buffer);
                input_buffer->reset();
                string result = output_buffer.str();
                ssize_t ret = send(fd, result.c_str(), result.size(), 0);
                if (ret != result.size())
                {
                    throw KernelException("sent less bytes than expected");
                }
            }
        });
    }

    HTTPServer::~HTTPServer()
    {
        map<int, InputBuffer *>::iterator it = input_buffer_map.begin();
        int count = 0;
        while (it != input_buffer_map.end())
        {
            if (it->second != NULL)
            {
                delete it->second;
            }
            it++;
            count++;
        }
        Logger::log(LogLevel::INFO, "deleted %d buffers", count);
    }

} // namespace fastev

#endif