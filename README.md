# Fastev
C++17 non-blocking event driven library to develop fast servers

Supported platforms: Linux, FreeBSD, macOS

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
    try
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
    catch (std::exception &e)
    {
        Logger::log(LogLevel::FATAL_ERROR, e.what());
    }
}
```

## Benchmark
Run simple HTTP server with disabled logging on MacBook Pro (15-inch, 2019)

```
wrk -t4 -c100 -d20 http://localhost:8080/benchmark
Running 20s test @ http://localhost:8080/benchmark
  4 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   618.21us   22.18us   1.43ms   76.37%
    Req/Sec    40.44k     1.09k   42.56k    63.31%
  3235418 requests in 20.10s, 317.81MB read
Requests/sec: 160965.09
Transfer/sec:     15.81MB
```