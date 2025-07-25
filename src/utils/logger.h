// src/utils/logger.h
#pragma once
#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>

namespace utec {

    enum class LogLevel {
        LOG_DEBUG,    // Changed from DEBUG to avoid macro conflicts
        LOG_INFO,     // Changed from INFO for consistency
        LOG_WARNING,  // Changed from WARNING for consistency
        LOG_ERROR     // Changed from ERROR for consistency
    };

    class Logger {
    public:
        static void log(LogLevel level, const std::string& message);
        static void debug(const std::string& message);
        static void info(const std::string& message);
        static void warning(const std::string& message);
        static void error(const std::string& message);

    private:
        static std::string getCurrentTime();
        static std::string levelToString(LogLevel level);
    };

} // namespace utec