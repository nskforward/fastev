#include "fastev/http/http_server.hpp"
using namespace fastev;

int main()
{
    try
    {
        auto s = HTTPServer(8080);
        s.onConnect([&](int fd, char *ip, int port) {
            Logger::log(LogLevel::INFO, "connect");
        });
        s.onDisconnect([&](int fd) {
            Logger::log(LogLevel::INFO, "disconnect");
        });
        s.onData([&](int fd, HTTPRequest *req, HTTPResponse *resp) {
            resp->setBody("OK");
        });
        s.start();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}