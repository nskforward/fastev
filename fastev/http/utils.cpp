#include "utils.hpp"

namespace fastev
{
    std::string http_code_to_str(HTTPCode code)
    {
        switch (code)
        {
        case HTTPCode::OK:
            return "200 OK";
        case HTTPCode::FOUND:
            return "302 Found";
        case HTTPCode::BAD_REQUEST:
            return "400 Bad Request";
        case HTTPCode::UNAUTHORIZED:
            return "401 Unauthorized";
        case HTTPCode::FORBIDDEN:
            return "403 Forbidden";
        case HTTPCode::NOT_FOUND:
            return "404 Not Found";
        case HTTPCode::REQUEST_TIMEOUT:
            return "408 Request Timeout";
        case HTTPCode::INTERNAL_SERVER_ERROR:
            return "500 Internal Server Error";
        case HTTPCode::SERVICE_UNAVAILABLE:
            return "503 Server Unavailable";
        default:
            throw KernelException("Unknown HTTP code: %d", code);
        }
    }
} // namespace fastev