#include "fastev/http/server.hpp"
using namespace fastev;

int main()
{
    try
    {
        auto s = HTTPServer(8080);
        s.onRequest([&](int fd, ByteBuffer *buf) {
            s.httpReply(fd, HTTPCode::OK, "text/html;charset=UTF-8", buf->getURI());
        });
        s.start();
    }
    catch (std::exception &e)
    {
        Logger::log(LogLevel::FATAL_ERROR, e.what());
    }
}