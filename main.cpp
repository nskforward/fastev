#include "fastev/http/server.hpp"

using namespace fastev;

int main()
{
    try
    {
        auto s = HTTPServer(8080);
        s.onConnect([](int fd, struct sockaddr &addr) {
            Logger::log(LogLevel::INFO, "connected");
        });
        s.onDisonnect([](int fd) {
            Logger::log(LogLevel::INFO, "disconnected");
        });
        s.start([](InputBuffer *req, OutputBuffer *resp) {
            resp->setHeader("Content-Type", "text/html;charset=UTF-8");
            resp->body() << req->getMethod() << " " << req->getURI();
        });
    }
    catch (std::exception &e)
    {
        Logger::log(LogLevel::FATAL_ERROR, e.what());
    }
}