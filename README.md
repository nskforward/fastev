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
    auto s = HTTPServer(8080);
    s.onRequest([&](int fd, ByteBuffer *buf) {
        s.httpReply(fd, HTTPCode::OK, "text/html;charset=UTF-8", buf->getURI());
    });
    s.start();
}
```

## Benchmark
Test - a http server that always returns uri request is called. Logging is disable. Tests ran on the same machine.
The goal of benchmark is to show the difference, not the absilute values.

### 1. HTTP server based on libevent
```
wrk -t5 -c100 -d20 http://localhost:8080/benchmark
Running 20s test @ http://localhost:8080/benchmark
  5 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.25ms   81.27us   2.60ms   85.53%
    Req/Sec    16.07k   458.11    17.15k    78.91%
  1607146 requests in 20.10s, 193.12MB read
Requests/sec:  79954.33
Transfer/sec:      9.61MB
```

### 2. HTTP server based on fastev
```
wrk -t5 -c100 -d20 http://localhost:8080/benchmark
Running 20s test @ http://localhost:8080/benchmark
  5 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   133.28ms  123.85ms   1.94s    84.31%
    Req/Sec   357.68k   106.04k  634.51k    67.71%
  35703395 requests in 20.11s, 3.66GB read
  Socket errors: connect 0, read 75, write 0, timeout 76
Requests/sec: 1775512.56
Transfer/sec:    186.26MB
```


## How to build
- install g++ make
- open project folder in console
- launch 'make compile'
- launch './build/app'