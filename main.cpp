#include "fastev/http/server.hpp"
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
        s.onRequest([&](HTTPResponse &resp) {
            resp << "hello world!";
            if (resp.getRequest()->body != "")
            {
                resp << resp.getRequest()->body;
            }
        });
        s.start();
    }
    catch (std::exception &e)
    {
        Logger::log(LogLevel::CRITICAL_ERROR, e.what());
    }
}