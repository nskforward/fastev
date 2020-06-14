#ifndef FASTEV_HTTP_REQUEST
#define FASTEV_HTTP_REQUEST

#include <string>
#include <map>

namespace fastev
{
    class HTTPRequest
    {
    private:
        std::string method;
        std::string uri;
        std::string proto;
        std::map<std::string, std::string> headers;

        void parseHeadLine(std::string line);
        void parseHeader(std::string line);

    public:
        HTTPRequest(std::string raw_headers);

        std::string getMethod();
        std::string getURI();
        std::string getHeader(std::string name);
    };

} // namespace fastev

#endif