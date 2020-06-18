#ifndef FASTEV_HTTP_EXCEPTION_HPP
#define FASTEV_HTTP_EXCEPTION_HPP

#include <exception>
#include "../core/format.hpp"
#include <stdarg.h>
#include <stdio.h>

namespace fastev
{
    class HTTPParserException : public std::exception
    {
    private:
        std::string body;

    public:
        template <typename... Args>
        HTTPParserException(const std::string format, Args... args)
        {
            body = Format::str(format, args...);
        }

        const char *what() const throw()
        {
            return body.c_str();
        }
    };
} // namespace fastev

#endif