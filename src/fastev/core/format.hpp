#ifndef FASTEV_FORMAT_HPP
#define FASTEV_FORMAT_HPP

#include <string>
#include <iostream>
#include <memory>
#include <string.h>
#include <stdarg.h>

namespace fastev
{
    class Format
    {
    public:
        static std::string str(const std::string format, ...)
        {
            int final_n, n = ((int)format.size()) * 2;
            std::unique_ptr<char[]> formatted;
            va_list ap;
            while (1)
            {
                formatted.reset(new char[n]);
                strcpy(&formatted[0], format.c_str());
                va_start(ap, format);
                final_n = vsnprintf(&formatted[0], n, format.c_str(), ap);
                va_end(ap);
                if (final_n < 0 || final_n >= n)
                    n += abs(final_n - n + 1);
                else
                    break;
            }
            return std::string(formatted.get());
        }
    };
} // namespace fastev

#endif