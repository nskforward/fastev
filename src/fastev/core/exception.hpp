#ifndef FASTEV_EXCEPTION_HPP
#define FASTEV_EXCEPTION_HPP

#include <exception>
#include "format.hpp"
#include <stdarg.h>
#include <stdio.h>

namespace fastev
{
    class KernelException : public std::exception
    {
    private:
        std::string body;

    public:
        template <typename... Args>
        KernelException(const std::string format, Args... args)
        {
            body = Format::str(format, args...);
            //body = "KernelException: " + str;
        }

        const char *what() const throw()
        {
            return body.c_str();
        }
    };
} // namespace fastev

#endif