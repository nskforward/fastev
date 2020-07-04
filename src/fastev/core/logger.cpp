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
        case LogLevel::FATAL_ERROR:
            return "FATAL";
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

    void Logger::write(LogLevel level, std::string &message)
    {
        mtx.lock();
        std::cout << std::put_time(getTime(), "[%Y-%m-%d %X]") << " -" << levelStr(level) << "- " << message << std::endl;
        mtx.unlock();
    }
} // namespace fastev