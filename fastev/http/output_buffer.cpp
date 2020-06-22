#include "output_buffer.hpp"
#include "utils.hpp"

namespace fastev
{
    OutputBuffer::OutputBuffer()
    {
        reset();
    }
    void OutputBuffer::setCode(HTTPCode code)
    {
        _code = code;
    }

    void OutputBuffer::reset()
    {
        _data[0] = '\0';
    }

    char *OutputBuffer::getData()
    {
        return _data;
    }

    void OutputBuffer::setHeader(std::string name, std::string value)
    {
        _headers[name] = value;
    }

    map<string, string> &OutputBuffer::getHeaders()
    {
        return _headers;
    }

    string OutputBuffer::str(int code, map<string, string> &headers, stringstream &body)
    {
        headers["Content-Length"] = std::to_string(body.str().size());
        std::stringstream ss;
        ss << "HTTP/1.1 " << http_code_to_str(code) << "\r\n";
        std::map<std::string, std::string>::iterator it = headers.begin();
        while (it != headers.end())
        {
            ss << it->first << ": " << it->second << "\r\n";
            it++;
        }
        ss << "\r\n"
           << body.str();
        return ss.str();
    }

    HTTPCode OutputBuffer::getCode()
    {
        return _code;
    }

} // namespace fastev