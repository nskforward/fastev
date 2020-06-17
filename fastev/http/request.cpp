#include "request.hpp"

namespace fastev
{
    HTTPRequest::HTTPRequest() {}

    HTTPRequest::HTTPRequest(Buffer *buf)
    {
        const char *source = buf->c_str();
        string raw_headers = string(source, buf->delimiter() + 2);
        _body = string(source + buf->delimiter() + 4, buf->size() - buf->delimiter() - 4);
        parse(&raw_headers);
    }

    void HTTPRequest::parse(string *raw_headers)
    {
        stringstream ss(*raw_headers);
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

    void HTTPRequest::parseHeadLine(string line)
    {
        stringstream ss(line);
        for (string word; getline(ss, word, ' ');)
        {
            if (_method == "")
            {
                _method = word;
                continue;
            }
            if (_uri == "")
            {
                _uri = word;
                continue;
            }
            if (_proto == "")
            {
                _proto = word;
                continue;
            }
        }
        if (_uri.size() > 1 && _uri.at(_uri.size() - 1) == '/')
        {
            _uri.resize(_uri.size() - 1);
        }
    }

    void HTTPRequest::parseHeader(string line)
    {
        size_t previous = 0;
        size_t current = line.find(": ");
        string name = line.substr(previous, current - previous);
        string value = line.substr(current + 2, line.size() - current + 2);
        _headers[name] = value;
    }

    string HTTPRequest::getMethod()
    {
        return _method;
    }

    string HTTPRequest::getURI()
    {
        return _uri;
    }

    string HTTPRequest::getProto()
    {
        return _proto;
    }

    string HTTPRequest::getHeader(string name)
    {
        return _headers[name];
    }

    string HTTPRequest::getBody()
    {
        return _body;
    }
} // namespace fastev