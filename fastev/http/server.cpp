#include "server.hpp"

namespace fastev
{
    HTTPServer::HTTPServer(int port) : TCPServer(port)
    {
        /*
        Reactor::onTimer([this]() {
            HTTPServer::onTimer();
        });
        */
        TCPServer::onChunk([this](int fd, char *data, size_t size) {
            HTTPServer::onChunk(fd, data, size);
        });
    }

    HTTPServer::~HTTPServer()
    {
        map<int, Buffer *>::iterator it = buffer_map.begin();
        while (it != buffer_map.end())
        {
            if (it->second != NULL)
            {
                delete it->second;
            }
            Logger::log(LogLevel::TRACE, "memory free [%d]", it->first);
            it++;
        }
    }

    void HTTPServer::onRequest(function<void(HTTPResponse &resp)> func)
    {
        request_cb = func;
    }

    void HTTPServer::onTimer()
    {
    }

    void HTTPServer::onChunk(int fd, char *data, size_t size)
    {

        if (buffer_map.count(fd) == 0 || buffer_map[fd] == NULL) // needs to init new buffer
        {
            buffer_map[fd] = buffer_pool.get();
        }
        Buffer *buf = buffer_map[fd];
        buf->append(data, size);
        if (buf->isFull())
        {
            onBufferFull(fd, buf);
            buf->reset();
            buffer_pool.put(buf);
            buffer_map[fd] = NULL;
        }
    }

    void HTTPServer::onBufferFull(int fd, Buffer *buf)
    {
        auto t1 = chrono::system_clock::now();
        auto req = HTTPRequest(buf);
        auto resp = HTTPResponse(req);
        resp.setCode(HTTPCode::OK);
        resp.setHeader("Host", req.getHeader("Host"));
        resp.setHeader("Content-Type", "text/html;charset=UTF-8");
        try
        {
            request_cb(resp);
        }
        catch (exception &e)
        {
            resp.setCode(HTTPCode::INTERNAL_SERVER_ERROR);
            resp.body() << "Internal Server Error";
            Logger::log(LogLevel::ERROR, e.what());
        }
        auto duration = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - t1);
        tcpReply(fd, resp.str().c_str(), resp.str().size());
        //Logger::log(LogLevel::INFO, "%d %s \"%s\" %dμs %s", resp.getCode(), req.getMethod().c_str(), req.getURI().c_str(), duration, req.getHeader("Host").c_str());
    }

} // namespace fastev
