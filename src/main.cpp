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
            if (strcmp(req.method(), "POST") == 0)
            {
                unordered_map<string, string> headers;
                headers["Content-Type"] = "text/plain;charset=utf-8";
                req.answer(200, req.body()->c_str(), headers);
                return;
            }
            req.answer("<form method=\"post\"><input type=\"text\" name=\"param1\"><input type=\"text\" name=\"param2\"><button type=\"submit\">Submit</button></form>");
        });
        s.start();
    }
    catch (std::exception &e)
    {
        Logger::log(LogLevel::FATAL_ERROR, e.what());
    }
}