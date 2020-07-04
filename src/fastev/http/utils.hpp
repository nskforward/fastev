#ifndef FASTEV_HTTP_HPP
#define FASTEV_HTTP_HPP

#include "../core/exception.hpp"

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

    std::string http_code_to_str(int code);
} // namespace fastev

#endif