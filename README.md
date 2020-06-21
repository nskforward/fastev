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
        s.start([](InputBuffer *req, OutputBuffer *resp) {
            resp->setHeader("Content-Type", "text/html;charset=UTF-8");
            resp->body() << req->getMethod() << " " << req->getURI();
        });
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
wrk -t10 -c100 -d20 http://localhost:8080/benchmark
Running 20s test @ http://localhost:8080/benchmark
  10 threads and 100 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     1.34ms    0.88ms  70.67ms   99.85%
    Req/Sec     7.60k   249.56     8.18k    82.69%
  1520768 requests in 20.10s, 165.34MB read
Requests/sec:  75653.08
Transfer/sec:      8.22MB

```

Fastev is able to serve x20 req/sec more than libevent on the same resources.


## How to build
- install g++ make
- open project folder in console
- launch 'make compile'
- launch './build/app'