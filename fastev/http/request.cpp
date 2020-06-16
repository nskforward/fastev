#include "request.hpp"

namespace fastev
{
    HTTPRequest::HTTPRequest() {}

    HTTPRequest::HTTPRequest(std::string raw_headers)
    {
        parse(raw_headers);
    }

    void HTTPRequest::reset()
    {
        body.clear();
        headers.clear();
        method.clear();
        uri.clear();
        proto.clear();
    }

    void HTTPRequest::parse(std::string &raw_headers)
    {
        stringstream ss(raw_headers);
        bool parsed_headline = false;
        for (string line; getline(ss, line);)
        {
            line.erase(line.find_last_not_of("\r") + 1);
            if (!parsed_headline)
            {
                parseHeadLine(line);
                parsed_headline = true;
                continue;
            }
            if (line == "")
            {
                break;
            }
            parseHeader(line);
        }
    }

    void HTTPRequest::parseHeadLine(std::string line)
    {
        stringstream ss(line);
        string word;
        getline(ss, word, ' ');
        method = word;
        getline(ss, word, ' ');
        uri = word;
        if (uri.size() > 1 && uri.at(uri.size() - 1) == '/')
        {
            uri.resize(uri.size() - 1);
        }
        getline(ss, word, ' ');
        proto = word;
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