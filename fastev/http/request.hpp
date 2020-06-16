#ifndef FASTEV_HTTP_REQUEST
#define FASTEV_HTTP_REQUEST

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include "../core/exception.hpp"

using namespace std;

namespace fastev
{
    class HTTPRequest
    {
    private:
        string method;
        string uri;
        string proto;
        map<std::string, std::string> headers;

        void parseHeadLine(string line);
        void parseHeader(string line);

    public:
        std::string body;

        HTTPRequest();
        HTTPRequest(string raw_headers);
        void reset();
        void parse(string &raw_headers);
        string getMethod();
        string getURI();
        string getHeader(string name);
    };

} // namespace fastev

#endif