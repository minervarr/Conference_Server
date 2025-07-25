// src/utils/logger.cpp
#include "utils/logger.h"
#include <sstream>

namespace utec {

    void Logger::log(LogLevel level, const std::string& message) {
        std::cout << "[" << getCurrentTime() << "] "
                  << "[" << levelToString(level) << "] "
                  << message << std::endl;
    }

    void Logger::debug(const std::string& message) {
        log(LogLevel::LOG_DEBUG, message);
    }

    void Logger::info(const std::string& message) {
        log(LogLevel::LOG_INFO, message);
    }

    void Logger::warning(const std::string& message) {
        log(LogLevel::LOG_WARNING, message);
    }

    void Logger::error(const std::string& message) {
        log(LogLevel::LOG_ERROR, message);
    }

    std::string Logger::getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

    std::string Logger::levelToString(LogLevel level) {
        switch (level) {
            case LogLevel::LOG_DEBUG:   return "DEBUG";
            case LogLevel::LOG_INFO:    return "INFO ";
            case LogLevel::LOG_WARNING: return "WARN ";
            case LogLevel::LOG_ERROR:   return "ERROR";
            default:                    return "UNKNOWN";
        }
    }

} // namespace utec