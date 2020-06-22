#include "utils.hpp"

namespace fastev
{
    std::string http_code_to_str(int code)
    {
        switch (code)
        {
        case 200:
            return "200 OK";
        case 302:
            return "302 Found";
        case 400:
            return "400 Bad Request";
        case 401:
            return "401 Unauthorized";
        case 403:
            return "403 Forbidden";
        case 404:
            return "404 Not Found";
        case 408:
            return "408 Request Timeout";
        case 500:
            return "500 Internal Server Error";
        case 503:
            return "503 Server Unavailable";
        default:
            throw KernelException("Unknown HTTP code: %d", code);
        }
    }
} // namespace fastev