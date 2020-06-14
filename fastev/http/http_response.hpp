#ifndef FASTEV_HTTP_RESPONSE
#define FASTEV_HTTP_RESPONSE

#include <string>
#include <sstream>
#include <map>
#include <iterator>

namespace fastev
{
    enum HTTPCode
    {
        OK = 200,
        FOUND = 302,
        BAD_REQUEST = 400,
        UNAUTHORIZED = 401,
        FORBIDDEN = 403,
        NOT_FOUND = 404,
        REQUEST_TIMEOUT = 408,
        INTERNAL_SERVER_ERROR = 500,
        SERVICE_UNAVAILABLE = 503
    };

    class HTTPResponse
    {
    private:
        HTTPCode _code;
        std::string _body;
        std::map<std::string, std::string> _headers;

    public:
        void setCode(HTTPCode code);
        void setBody(std::string body);
        void setHeader(std::string name, std::string value);
        std::string str();
        HTTPCode getCode();
    };

} // namespace fastev
#endif