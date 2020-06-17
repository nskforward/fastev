#include "fastev/http/server.hpp"
using namespace fastev;

int main()
{
    auto s = HTTPServer(8080);
    s.onConnect([&](int fd, char *ip, int port) {
        Logger::log(LogLevel::INFO, "connect");
    });
    s.onDisconnect([&](int fd) {
        Logger::log(LogLevel::INFO, "disconnect");
    });
    s.onRequest([&](HTTPResponse &resp) {
        resp << "OK";
    });
    s.start();
}