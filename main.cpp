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
        s.start([](InputBuffer *input_buffer, map<string, string> &response_headers, stringstream &response_body) {
            response_headers["Content-Type"] = "text/html;charset=UTF-8";
            response_body << "OK";
            return 200;
        });
    }
    catch (std::exception &e)
    {
        Logger::log(LogLevel::FATAL_ERROR, e.what());
    }
}