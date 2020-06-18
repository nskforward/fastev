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
        map<int, ByteBuffer *>::iterator it = buffer_map.begin();
        while (it != buffer_map.end())
        {
            if (it->second != NULL)
            {
                delete it->second;
            }
            it++;
        }
    }

    void HTTPServer::onChunk(int fd, char *data, size_t size)
    {

        if (buffer_map.count(fd) == 0 || buffer_map[fd] == NULL) // needs to init new buffer
        {
            buffer_map[fd] = buffer_pool.get();
        }
        ByteBuffer *buf = buffer_map[fd];
        try
        {
            buf->append(data, size);
        }
        catch (HTTPParserException &e)
        {
            Logger::log(LogLevel::ERROR, e.what());
            disconnect(fd);
            buffer_pool.put(buf);
            buffer_map[fd] = NULL;
            return;
        }
        if (buf->isFull())
        {
            onBufferFull(fd, buf);
            buffer_pool.put(buf);
            buffer_map[fd] = NULL;
        }
    }

    void HTTPServer::onRequest(function<void(int fd, ByteBuffer *buf)> func)
    {
        request_cb = func;
    }

    void HTTPServer::onTimer()
    {
    }

    void HTTPServer::onBufferFull(int fd, ByteBuffer *buf)
    {
        //auto t1 = chrono::system_clock::now();
        try
        {
            request_cb(fd, buf);
        }
        catch (exception &e)
        {
            httpReply(fd, HTTPCode::INTERNAL_SERVER_ERROR, "text/html;charset=UTF-8", "Internal Server Error");
            Logger::log(LogLevel::ERROR, e.what());
        }
        //auto duration = chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now() - t1);
        //Logger::log(LogLevel::INFO, "%d %s \"%s\" %dμs %s", resp.getCode(), req.getMethod().c_str(), req.getURI().c_str(), duration, req.getHeader("Host").c_str());
    }

    void HTTPServer::httpReply(int fd, HTTPCode code, string type, string body)
    {
        /*
        char buffer[2048];
        int size = sprintf(buffer, "HTTP/1.1 %s\r\nHost: localhost:8080\r\nContent-Type: %s\r\nContent-Length: %lu\r\n\r\n%s", http_code_to_str(code).c_str(), type.c_str(), body.size(), body.c_str());
        */
        reply_buf.clear();
        reply_buf << "HTTP/1.1 " << http_code_to_str(code) << "\r\nHost: localhost:8080\r\nContent-Type: " << type << "\r\nContent-Length: " << body.size() << "\r\n\r\n"
                  << body;
        tcpReply(fd, reply_buf.str().c_str(), reply_buf.str().size());
    }

} // namespace fastev
