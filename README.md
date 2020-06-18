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
        auto s = HTTPServer(8080);
        s.onRequest([&](int fd, ByteBuffer *buf) {
            s.httpReply(fd, HTTPCode::OK, "text/html;charset=UTF-8", buf->getURI());
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
Testing of 2 HTTP servers based on different libraries to see the difference.
The test scenario is returning called URI (to involve http parser).

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
    Latency   162.62ms  134.40ms   1.82s    74.54%
    Req/Sec   358.51k   131.56k  775.48k    70.77%
  35756203 requests in 20.09s, 3.66GB read
Requests/sec: 1779518.22
Transfer/sec:    186.68MB

```

Fastev is able to serve x20 req/sec more than libevent on the same resources.


## How to build
- install g++ make
- open project folder in console
- launch 'make compile'
- launch './build/app'