# libfastev
C++17 non-blocking event driven library to develop fast servers

Supported platforms: macOS (kqueue), Linux (epoll)

No external dependencies

Zero memory allocations in hot paths


## TCP echo server

```cp
#include "fastev/tcp/server.hpp"
using namespace fastev;

int main()
{
    try
    {
        auto s = TCPServer(8080);
        s.onConnect([&](int fd, char *ip, int port) {
            Logger::log(LogLevel::INFO, "connect");
        });
        s.onDisconnect([&](int fd) {
            Logger::log(LogLevel::INFO, "disconnect");
        });
        s.onChunk([&](int fd, char *message, size_t size) {
            Logger::log(LogLevel::INFO, "receive %d bytes", size);
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
#include "fastev/http/server.hpp"
using namespace fastev;

int main()
{
    try
    {
        auto app = HTTPServer(8080);
        app.registryHandler("/", [&](HTTPResponse &resp) {
            resp << "home";
        });
        app.registryHandler("/about", [&](HTTPResponse &resp) {
            resp << "about";
        });
        app.start();
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