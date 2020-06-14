# fastevent
C++17 non-blocking event driven library to develop fast servers

Supported platforms: macOS (kqueue), Linux (epoll)

No external dependencies

Zero memory allocations in hot paths


## TCP echo server

```cp
#include "fastev/tcp/tcp_server.hpp"

int main()
{
    try
    {
        auto s = fastev::TCPServer(8080);
        s.onConnect([&](int fd, char *ip, int port) {
            std::cout << "connect" << std::endl;
        });
        s.onDisconnect([&](int fd) {
            std::cout << "disconnect" << std::endl;
        });
        s.onData([&](int fd, char *message, size_t size) {
            std::cout << "data" << std::endl;
            s.tcpReply(fd, message, size);
        });
        s.start();
    }
    catch (std::exception &e)
    {
        std::cout << e.what() << std::endl;
    }
}
```

## Simple HTTP server

```cp
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
```

## How to build
- install g++ make
- open project folder in console
- launch 'make compile'
- launch './build/app'