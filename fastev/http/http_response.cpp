#include "http_response.hpp"

namespace fastev
{
    std::string HTTPCodeToStr(HTTPCode code)
    {
        switch (code)
        {
        case HTTPCode::OK:
            return "OK";

        case HTTPCode::FOUND:
            return "Found";

        case HTTPCode::BAD_REQUEST:
            return "Bad Request";

        case HTTPCode::UNAUTHORIZED:
            return "Unauthorized";

        case HTTPCode::FORBIDDEN:
            return "Forbidden";

        case HTTPCode::NOT_FOUND:
            return "Not Found";

        case HTTPCode::REQUEST_TIMEOUT:
            return "Request Timeout";

        case HTTPCode::INTERNAL_SERVER_ERROR:
            return "Internal Server Error";

        case HTTPCode::SERVICE_UNAVAILABLE:
            return "Service Unavailable";

        default:
            return "Unknown";
        }
    }

    void HTTPResponse::setCode(HTTPCode code)
    {
        _code = code;
    }

    std::stringstream &HTTPResponse::body()
    {
        return _body;
    }

    void HTTPResponse::setHeader(std::string name, std::string value)
    {
        _headers[name] = value;
    }

    std::string HTTPResponse::str()
    {
        setHeader("Content-Length", std::to_string(_body.str().size()));
        std::ostringstream ss;
        ss << "HTTP/1.1 " << _code << " " << HTTPCodeToStr(_code) << "\r\n";
        std::map<std::string, std::string>::iterator it = _headers.begin();
        while (it != _headers.end())
        {
            ss << it->first << ": " << it->second << "\r\n";
            it++;
        }
        ss << "\r\n"
           << _body.str();
        return ss.str();
    }

    HTTPCode HTTPResponse::getCode()
    {
        return _code;
    }

} // namespace fastev