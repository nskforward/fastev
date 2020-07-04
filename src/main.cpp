#include "fastev/http/server.hpp"

using namespace fastev;

int main()
{
    auto s = HTTPServer(8080);
    s.onConnect([](int fd, struct sockaddr &addr) {
        char ip[16];
        Logger::log(LogLevel::INFO, "[fd:%d] connect from %s", fd, TCPSocket::addrToStr(addr, ip));
    });
    s.onDisonnect([](int fd, int worker_id) {
        Logger::log(LogLevel::INFO, "[fd:%d] disconnected", fd);
    });
    s.onRequest([](Request &req) {
        Logger::log(LogLevel::INFO, "[fd:%d] %s %s %s", req.fd(), req.worker_id(), req.method(), req.uri(), req.proto_ver());
        req.answer("OK");
    });
    s.start();
}