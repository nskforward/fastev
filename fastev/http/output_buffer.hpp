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
        char _data[8192];
        HTTPCode _code = HTTPCode::OK;
        map<string, string> _headers;

    public:
        void setCode(HTTPCode code);
        char *getData();
        void setHeader(string name, string value);
        string str(int code, map<string, string> &headers, stringstream &ss);
        HTTPCode getCode();
        map<string, string> &getHeaders();
        void reset();
        OutputBuffer();
    };

} // namespace fastev
#endif