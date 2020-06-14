#include "http_server.hpp"

namespace fastev
{
    HTTPServer::HTTPServer(int port) : TCPServer(port)
    {
        TCPServer::onTimer([this]() {
            onTimer();
        });
    }

    void HTTPServer::onTimer()
    {
        std::map<int, Buffer *>::iterator it = buffer_map.begin();
        while (it != buffer_map.end())
        {
            int fd = it->first;
            Buffer *buf = it->second;
            if (buf != NULL && buf->older(FASTEV_HTTP_READ_TIMEOUT))
            {
                onReadTimeout(fd);
                buffer_pool.put(buf);
                buffer_map[fd] = NULL;
            }
            it++;
        }
    }

    void HTTPServer::onReadTimeout(int fd)
    {
        disconnect(fd);
        Logger::log(LogLevel::INFO, "read timeout (%d seconds) on fd:%d", FASTEV_HTTP_READ_TIMEOUT, fd);
    }

    void HTTPServer::onData(std::function<void(int fd, HTTPRequest *req, HTTPResponse *resp)> func)
    {
        on_data_cb = func;
    }

    void HTTPServer::start()
    {
        TCPServer::onData([this](int fd, char *data, size_t size) {
            onTCPData(fd, data, size);
        });
        TCPServer::start();
    }

    void HTTPServer::onTCPData(int fd, char *data, size_t size)
    {
        Buffer *buf = NULL;
        if (buffer_map.count(fd) == 0 || buffer_map[fd] == NULL)
        {
            buffer_map[fd] = buffer_pool.get();
        }
        buf = buffer_map[fd];
        if (size > 3 && data[size - 1] == '\n' && data[size - 2] == '\r' && data[size - 3] == '\n' && data[size - 4] == '\r')
        {
            buf->append(data, size - 2);
            onHeaders(fd, buf->str());
            buffer_pool.put(buf);
            buffer_map[fd] = NULL;
            return;
        }
        buf->append(data, size);
    };

    void HTTPServer::onHeaders(int fd, std::string raw_headers)
    {
        auto t1 = std::chrono::system_clock::now();
        HTTPRequest req = HTTPRequest(raw_headers);
        HTTPResponse resp = HTTPResponse();
        resp.setCode(HTTPCode::OK);
        resp.setHeader("Host", req.getHeader("Host"));
        resp.setHeader("Content-Type", "text/html;charset=UTF-8");
        try
        {
            on_data_cb(fd, &req, &resp);
        }
        catch (std::exception &e)
        {
            resp.setCode(HTTPCode::INTERNAL_SERVER_ERROR);
            resp.setBody("Internal Server Error");
            Logger::log(LogLevel::ERROR, e.what());
        }
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - t1);
        tcpReply(fd, resp.str().c_str(), resp.str().size());
        Logger::log(LogLevel::INFO, "%d %s \"%s\" %dμs %s", resp.getCode(), req.getMethod().c_str(), req.getURI().c_str(), duration, req.getHeader("Host").c_str());
    }

} // namespace fastev