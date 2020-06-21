#ifndef FASTEV_HTTP_OUTPUT_BUFFER
#define FASTEV_HTTP_OUTPUT_BUFFER

#include <string>
#include <sstream>
#include <map>
#include <iterator>
#include "utils.hpp"

using namespace std;

namespace fastev
{
    class OutputBuffer
    {
    private:
        HTTPCode _code = HTTPCode::OK;
        stringstream _body;
        map<string, string> _headers;

    public:
        OutputBuffer(string host);
        void setCode(HTTPCode code);
        stringstream &body();
        void setHeader(string name, string value);
        string str();
        HTTPCode getCode();
        void clearBody();

        OutputBuffer &operator<<(string src)
        {
            _body << src;
            return *this;
        }
    };

} // namespace fastev
#endif