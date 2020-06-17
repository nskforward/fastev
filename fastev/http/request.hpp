#ifndef FASTEV_HTTP_REQUEST
#define FASTEV_HTTP_REQUEST

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include "../core/exception.hpp"
#include "buffer.hpp"

using namespace std;

namespace fastev
{
    class HTTPRequest
    {
    private:
        string _method;
        string _uri;
        string _proto;
        string _body;
        map<string, string> _headers;

        void parseHeadLine(string line);
        void parseHeader(string line);

    public:
        HTTPRequest();
        HTTPRequest(Buffer *buf);
        void parse(string *raw_headers);
        string getMethod();
        string getURI();
        string getProto();
        string getBody();
        string getHeader(string name);
    };

} // namespace fastev

#endif