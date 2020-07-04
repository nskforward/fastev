# Fastev
C++17 non-blocking event driven library to develop fast servers

Supported platforms: macOS (kqueue), Linux (epoll)

No external libraries

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
        s.onConnect([](int fd, struct sockaddr &addr) {
            Logger::log(LogLevel::INFO, "connected");
        });
        s.onDisonnect([](int fd) {
            Logger::log(LogLevel::INFO, "disconnected");
        });
        s.onChunk([&](int fd, int worker_id, const char *chunk, size_t size) {
            Logger::log(LogLevel::INFO, "read %d bytes", size);
            send(fd, chunk, size, 0);
        });
        s.start();
    }
    catch (std::exception &e)
    {
        Logger::log(LogLevel::FATAL_ERROR, e.what());
    }
}
```

## Simple HTTP server

```cp
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
        Logger::log(LogLevel::INFO, "[fd:%d] %s %s %s", req.fd(), req.method(), req.uri(), req.proto_ver());
        req.answer("OK");
    });
    s.start();
}
```

## Benchmark
Run simple HTTP server with disabled logging on MacBook Pro (15-inch, 2019)

```
wrk -t4 -c100 -d20 http://localhost:8080/benchmark
Running 20s test @ http://localhost:8080/benchmark
  4 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   638.99us   59.19us   1.61ms   88.06%
    Req/Sec    39.08k     2.90k   49.98k    75.97%
  3123299 requests in 20.10s, 235.31MB read
Requests/sec: 155388.43
Transfer/sec:     11.71MB
```

## How to build
- install g++ make
- open project folder in console
- launch 'make compile'
- launch './build/app'