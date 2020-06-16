#ifndef FASTEV_LOGGER_HPP
#define FASTEV_LOGGER_HPP

#include "format.hpp"
#include <iomanip>
#include <iostream>
#include <chrono>

#ifndef FASTEV_LOG_LEVEL
#define FASTEV_LOG_LEVEL 0
#endif

namespace fastev
{
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        CRITICAL_ERROR,
    };

    class Logger
    {
    private:
        static std::string levelStr(LogLevel level);
        static tm *getTime();

    public:
        template <typename... Args>
        static void log(LogLevel level, const std::string format, Args... args)
        {
            if (level < FASTEV_LOG_LEVEL)
            {
                return;
            }
            auto str = Format::str(format, args...);
            std::cout << std::put_time(getTime(), "[%Y-%m-%d %X]") << " -" << levelStr(level) << "- " << str << std::endl;
        }
    };

} // namespace fastev

#endif