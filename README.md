# Fastev
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
        s.onConnect([](int fd, struct sockaddr &addr) {
            Logger::log(LogLevel::INFO, "connected");
        });
        s.onDisonnect([](int fd) {
            Logger::log(LogLevel::INFO, "disconnected");
        });
        s.start([](int fd, char *chunk, size_t size) {
            send(fd, chunk, size, 0);
        });
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
```

## Benchmark
Run simple HTTP server with disabled logging on MacBook Pro (15-inch, 2019)

```
wrk -t10 -c100 -d20 http://localhost:8080/benchmark
Running 20s test @ http://localhost:8080/benchmark
  10 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   694.11us  106.66us   2.12ms   80.81%
    Req/Sec    14.38k     1.36k   16.96k    76.82%
  2876233 requests in 20.10s, 277.04MB read
Requests/sec: 143098.52
Transfer/sec:     13.78MB
```

## How to build
- install g++ make
- open project folder in console
- launch 'make compile'
- launch './build/app'