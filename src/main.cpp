#include "fastev/http/server.hpp"

using namespace fastev;

int main()
{
    try
    {
        auto s = HTTPServer(8080);
        s.onConnect([](int fd, struct sockaddr &addr) {
            char ip[16];
            Logger::log(LogLevel::INFO, "[#%d] connect from %s", fd, TCPSocket::addrToStr(addr, ip));
        });
        s.onDisonnect([](int fd, int worker_id) {
            Logger::log(LogLevel::INFO, "[#%d] disconnect", fd);
        });
        s.onRequest([](Request &req) {
            req.answer("Home page");
        });
        s.start();
    }
    catch (std::exception &e)
    {
        Logger::log(LogLevel::FATAL_ERROR, e.what());
    }
}