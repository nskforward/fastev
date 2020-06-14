#include "logger.hpp"

namespace fastev
{
    std::string Logger::levelStr(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::TRACE:
            return "TRACE";
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARN";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::CRITICAL_ERROR:
            return "CRIT";
        default:
            return "UNKNOWN";
        }
    }

    tm *Logger::getTime()
    {
        auto input = std::chrono::system_clock::now();
        auto tt = std::chrono::system_clock::to_time_t(input);
        return std::gmtime(&tt);
    }
} // namespace fastev
