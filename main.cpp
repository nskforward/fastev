#include "fastev/http/http_server.hpp"
#include "fastev/html/page.hpp"
#include "fastev/html/link.hpp"
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
        s.onData([&](int fd, HTTPRequest &req, HTTPResponse &resp) {
            auto page = HTMLPage("tpl/layout.tpl");
            page.title("Home page");
            auto p = HTMLTag("p").body("This is");
            auto a = HTMLLink("/", "home page");
            page.body() << (p << a);
            resp << page;
        });
        s.start();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}