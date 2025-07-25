// src/core/error_handler.h
#pragma once
#include <stdexcept>
#include <string>

namespace utec {

    enum class ErrorCode {
        // File system errors
        FILE_NOT_FOUND = 1000,
        DIRECTORY_NOT_FOUND = 1001,
        INVALID_PATH = 1002,
        PERMISSION_DENIED = 1003,
        PATH_TRAVERSAL_ATTEMPT = 1004,

        // Server errors
        SERVER_START_FAILED = 2000,
        PORT_IN_USE = 2001,
        BIND_FAILED = 2002,

        // API errors
        INVALID_REQUEST = 3000,
        INVALID_PARAMETERS = 3001,
        RESOURCE_NOT_FOUND = 3002,

        // Configuration errors
        INVALID_CONFIG = 4000,
        MISSING_REQUIRED_PARAM = 4001,

        // Video errors
        UNSUPPORTED_FORMAT = 5000,
        FILE_TOO_LARGE = 5001,
        CORRUPTED_FILE = 5002,

        // Generic errors
        INTERNAL_ERROR = 9000,
        UNKNOWN_ERROR = 9999
    };

    class ServerException : public std::exception {
    public:
        ServerException(ErrorCode code, const std::string& message);
        ServerException(ErrorCode code, const std::string& message, const std::string& details);

        const char* what() const noexcept override;
        ErrorCode getCode() const noexcept;
        const std::string& getMessage() const noexcept;
        const std::string& getDetails() const noexcept;
        int getHttpStatus() const noexcept;

        // Factory methods for common errors
        static ServerException fileNotFound(const std::string& path);
        static ServerException invalidPath(const std::string& path);
        static ServerException pathTraversal(const std::string& path);
        static ServerException permissionDenied(const std::string& path);
        static ServerException serverStartFailed(int port, const std::string& reason = "");
        static ServerException invalidRequest(const std::string& details = "");
        static ServerException resourceNotFound(const std::string& resource);
        static ServerException unsupportedFormat(const std::string& format);

    private:
        ErrorCode code_;
        std::string message_;
        std::string details_;
        mutable std::string what_message_;

        void buildWhatMessage() const;
        int mapCodeToHttpStatus(ErrorCode code) const;
    };

    class ErrorHandler {
    public:
        static std::string formatErrorResponse(const ServerException& ex);
        static std::string formatErrorResponse(ErrorCode code, const std::string& message);
        static void logError(const ServerException& ex);
        static void logError(const std::string& context, const std::exception& ex);

    private:
        static std::string errorCodeToString(ErrorCode code);
    };

} // namespace utec