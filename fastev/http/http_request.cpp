#include "http_request.hpp"

namespace fastev
{
    HTTPRequest::HTTPRequest(std::string raw_headers)
    {
        size_t previous = 0;
        size_t current = raw_headers.find("\r\n");
        while (current != std::string::npos)
        {
            std::string line = raw_headers.substr(previous, current - previous);
            if (previous == 0)
            {
                parseHeadLine(line);
            }
            else
            {
                parseHeader(line);
            }
            previous = current + 2;
            current = raw_headers.find("\r\n", previous);
        }
    }

    void HTTPRequest::parseHeadLine(std::string line)
    {
        size_t previous = 0;
        size_t current = line.find(" ");
        method = line.substr(previous, current - previous);
        previous = current + 1;
        current = line.find(" ", previous);
        uri = line.substr(previous, current - previous);
        if (uri.size() > 1 && uri.at(uri.size() - 1) == '/')
        {
            uri.resize(uri.size() - 1);
        }
        previous = current + 1;
        current = line.size();
        proto = line.substr(previous, current - previous);
    }

    void HTTPRequest::parseHeader(std::string line)
    {
        size_t previous = 0;
        size_t current = line.find(": ");
        std::string name = line.substr(previous, current - previous);
        std::string value = line.substr(current + 2, line.size() - current + 2);
        headers[name] = value;
    }

    std::string HTTPRequest::getMethod()
    {
        return method;
    }

    std::string HTTPRequest::getURI()
    {
        return uri;
    }

    std::string HTTPRequest::getHeader(std::string name)
    {
        return headers[name];
    }
} // namespace fastev