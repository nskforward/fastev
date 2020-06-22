#ifndef FASTEV_HTTP_SERVER
#define FASTEV_HTTP_SERVER
#include "../tcp/server.hpp"
#include "input_buffer_pool.hpp"
#include "output_buffer.hpp"
#include "../core/thread_pool.hpp"
#include <map>

using namespace std;

namespace fastev
{
    class HTTPServer : public TCPServer
    {
    private:
        InputBufferPool buffer_pool = InputBufferPool(16);
        function<void(InputBuffer *input_buffer, OutputBuffer *output_buffer)> on_request_cb;
        ThreadPool workers = ThreadPool(4);

    public:
        HTTPServer(int port);
        void start(function<int(InputBuffer *input_buffer, map<string, string> &response_headers, stringstream &response_body)> callback);
    };

    HTTPServer::HTTPServer(int port) : TCPServer(port) {}

    void HTTPServer::start(function<int(InputBuffer *input_buffer, map<string, string> &response_headers, stringstream &response_body)> callback)
    {
        TCPServer::start(
            [this]() {
                return (void *)buffer_pool.get();
            },
            [this](void *buff) {
                buffer_pool.put((InputBuffer *)buff);
            },
            [this, &callback](int fd, void *buff, char *chunk, size_t size) {
                InputBuffer *input_buffer = (InputBuffer *)buff;
                if (input_buffer->isFull())
                {
                    throw KernelException("previous request is not handled on the same fd");
                }
                input_buffer->append(chunk, size);
                if (input_buffer->isFull())
                {
                    workers.enqueue([&callback](
                                        int fd,
                                        InputBuffer *input_buffer) {
                        auto output = input_buffer->getOutputBuffer();
                        map<string, string> headers;
                        headers["Host"] = input_buffer->getHost();
                        stringstream response_body = stringstream(output->getData());
                        int code = callback(input_buffer, headers, response_body);
                        auto result = output->str(code, headers, response_body);
                        auto ret = send(fd, result.c_str(), result.size(), 0);
                        if (ret < result.size())
                        {
                            throw KernelException("partional send");
                        }
                        input_buffer->reset();
                        output->reset();
                    },
                                    fd, input_buffer);
                }
            });
    }
} // namespace fastev

#endif