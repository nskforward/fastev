#ifndef FASTEV_HTTP_RESPONSE
#define FASTEV_HTTP_RESPONSE

#include <string>
#include <sstream>
#include <map>
#include <iterator>
#include "http.hpp"
#include "request.hpp"

using namespace std;

namespace fastev
{
    class HTTPResponse
    {
    private:
        HTTPCode _code;
        stringstream _body;
        string response_buf;
        map<string, string> _headers;
        HTTPRequest _request;

    public:
        HTTPResponse(HTTPRequest &request);
        void setCode(HTTPCode code);
        stringstream &body();
        void setHeader(string name, string value);
        string str();
        HTTPCode getCode();
        HTTPRequest &getRequest();

        HTTPResponse &operator<<(string src)
        {
            _body << src;
            return *this;
        }
    };

} // namespace fastev
#endif