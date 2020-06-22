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
wrk -t4 -c100 -d20 http://localhost:8080/benchmark
Running 20s test @ http://localhost:8080/benchmark
  4 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   648.83us   89.45us  10.83ms   89.19%
    Req/Sec    38.33k     1.94k   42.45k    65.80%
  3066400 requests in 20.10s, 295.36MB read
Requests/sec: 152555.26
Transfer/sec:     14.69MB
```

## How to build
- install g++ make
- open project folder in console
- launch 'make compile'
- launch './build/app'