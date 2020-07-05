#ifndef FASTEV_HTTP_RESPONSE
#define FASTEV_HTTP_RESPONSE

#include <unordered_map>
#include <sstream>
#include "utils.hpp"

using namespace std;

namespace fastev
{
    class Response
    {
    private:
        int _code = 200;

    public:
        stringstream body;
        unordered_map<string, string> headers;
        string str();
        void setCode(int code);
    };

    void Response::setCode(int code)
    {
        _code = code;
    }

    string Response::str()
    {
        headers["Content-Length"] = std::to_string(body.str().size());
        std::stringstream ss;
        ss << "HTTP/1.1 " << http_code_to_str(_code) << "\r\n";
        std::unordered_map<std::string, std::string>::iterator it = headers.begin();
        while (it != headers.end())
        {
            ss << it->first << ": " << it->second << "\r\n";
            it++;
        }
        ss << "\r\n"
           << body.str();
        return ss.str();
    }

} // namespace fastev

#endif