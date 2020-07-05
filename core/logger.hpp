#ifndef FASTEV_CORE_LOGGER
#define FASTEV_CORE_LOGGER

#include "format.hpp"
#include <iomanip>
#include <iostream>
#include <chrono>
#include <mutex>

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
        FATAL_ERROR
    };

    class Logger
    {
    private:
        std::string levelStr(LogLevel level);
        tm *getTime();
        std::mutex mtx;
        void write(LogLevel level, std::string &message);
        Logger(){};
        static Logger &getInstance()
        {
            static Logger instance;
            return instance;
        }

    public:
        template <typename... Args>
        static void log(LogLevel level, const std::string format, Args... args)
        {
            if (level < FASTEV_LOG_LEVEL)
            {
                return;
            }
            auto str = Format::str(format, args...);
            getInstance().write(level, str);
        }
    };

} // namespace fastev

#endif