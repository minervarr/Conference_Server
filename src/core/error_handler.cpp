// src/core/error_handler.cpp
#include "core/error_handler.h"
#include "utils/logger.h"
#include "api/json_response.h"
#include <sstream>

namespace utec {

ServerException::ServerException(ErrorCode code, const std::string& message)
    : code_(code), message_(message) {
    buildWhatMessage();
}

ServerException::ServerException(ErrorCode code, const std::string& message, const std::string& details)
    : code_(code), message_(message), details_(details) {
    buildWhatMessage();
}

const char* ServerException::what() const noexcept {
    return what_message_.c_str();
}

ErrorCode ServerException::getCode() const noexcept {
    return code_;
}

const std::string& ServerException::getMessage() const noexcept {
    return message_;
}

const std::string& ServerException::getDetails() const noexcept {
    return details_;
}

int ServerException::getHttpStatus() const noexcept {
    return mapCodeToHttpStatus(code_);
}

void ServerException::buildWhatMessage() const {
    std::ostringstream oss;
    oss << "[" << static_cast<int>(code_) << "] " << message_;
    if (!details_.empty()) {
        oss << " (" << details_ << ")";
    }
    what_message_ = oss.str();
}

int ServerException::mapCodeToHttpStatus(ErrorCode code) const {
    switch (code) {
        case ErrorCode::FILE_NOT_FOUND:
        case ErrorCode::DIRECTORY_NOT_FOUND:
        case ErrorCode::RESOURCE_NOT_FOUND:
            return 404;

        case ErrorCode::PERMISSION_DENIED:
            return 403;

        case ErrorCode::INVALID_PATH:
        case ErrorCode::PATH_TRAVERSAL_ATTEMPT:
        case ErrorCode::INVALID_REQUEST:
        case ErrorCode::INVALID_PARAMETERS:
        case ErrorCode::MISSING_REQUIRED_PARAM:
        case ErrorCode::UNSUPPORTED_FORMAT:
            return 400;

        case ErrorCode::FILE_TOO_LARGE:
            return 413; // Payload Too Large

        case ErrorCode::SERVER_START_FAILED:
        case ErrorCode::PORT_IN_USE:
        case ErrorCode::BIND_FAILED:
        case ErrorCode::INTERNAL_ERROR:
        case ErrorCode::CORRUPTED_FILE:
        default:
            return 500;
    }
}

// Factory methods
ServerException ServerException::fileNotFound(const std::string& path) {
    return ServerException(ErrorCode::FILE_NOT_FOUND, "File not found", path);
}

ServerException ServerException::invalidPath(const std::string& path) {
    return ServerException(ErrorCode::INVALID_PATH, "Invalid path", path);
}

ServerException ServerException::pathTraversal(const std::string& path) {
    return ServerException(ErrorCode::PATH_TRAVERSAL_ATTEMPT, "Path traversal attempt detected", path);
}

ServerException ServerException::permissionDenied(const std::string& path) {
    return ServerException(ErrorCode::PERMISSION_DENIED, "Permission denied", path);
}

ServerException ServerException::serverStartFailed(int port, const std::string& reason) {
    std::string details = "Port " + std::to_string(port);
    if (!reason.empty()) {
        details += ": " + reason;
    }
    return ServerException(ErrorCode::SERVER_START_FAILED, "Failed to start server", details);
}

ServerException ServerException::invalidRequest(const std::string& details) {
    return ServerException(ErrorCode::INVALID_REQUEST, "Invalid request", details);
}

ServerException ServerException::resourceNotFound(const std::string& resource) {
    return ServerException(ErrorCode::RESOURCE_NOT_FOUND, "Resource not found", resource);
}

ServerException ServerException::unsupportedFormat(const std::string& format) {
    return ServerException(ErrorCode::UNSUPPORTED_FORMAT, "Unsupported file format", format);
}

// ErrorHandler implementation
std::string ErrorHandler::formatErrorResponse(const ServerException& ex) {
    return JsonResponse::createErrorResponse(ex.getMessage(), static_cast<int>(ex.getCode()));
}

std::string ErrorHandler::formatErrorResponse(ErrorCode code, const std::string& message) {
    return JsonResponse::createErrorResponse(message, static_cast<int>(code));
}

void ErrorHandler::logError(const ServerException& ex) {
    std::ostringstream oss;
    oss << "ServerException: " << ex.getMessage();
    if (!ex.getDetails().empty()) {
        oss << " (Details: " << ex.getDetails() << ")";
    }
    oss << " [Code: " << static_cast<int>(ex.getCode()) << "]";

    Logger::error(oss.str());
}

void ErrorHandler::logError(const std::string& context, const std::exception& ex) {
    std::ostringstream oss;
    oss << "Error in " << context << ": " << ex.what();
    Logger::error(oss.str());
}

std::string ErrorHandler::errorCodeToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::FILE_NOT_FOUND: return "FILE_NOT_FOUND";
        case ErrorCode::DIRECTORY_NOT_FOUND: return "DIRECTORY_NOT_FOUND";
        case ErrorCode::INVALID_PATH: return "INVALID_PATH";
        case ErrorCode::PERMISSION_DENIED: return "PERMISSION_DENIED";
        case ErrorCode::PATH_TRAVERSAL_ATTEMPT: return "PATH_TRAVERSAL_ATTEMPT";
        case ErrorCode::SERVER_START_FAILED: return "SERVER_START_FAILED";
        case ErrorCode::PORT_IN_USE: return "PORT_IN_USE";
        case ErrorCode::BIND_FAILED: return "BIND_FAILED";
        case ErrorCode::INVALID_REQUEST: return "INVALID_REQUEST";
        case ErrorCode::INVALID_PARAMETERS: return "INVALID_PARAMETERS";
        case ErrorCode::RESOURCE_NOT_FOUND: return "RESOURCE_NOT_FOUND";
        case ErrorCode::INVALID_CONFIG: return "INVALID_CONFIG";
        case ErrorCode::MISSING_REQUIRED_PARAM: return "MISSING_REQUIRED_PARAM";
        case ErrorCode::UNSUPPORTED_FORMAT: return "UNSUPPORTED_FORMAT";
        case ErrorCode::FILE_TOO_LARGE: return "FILE_TOO_LARGE";
        case ErrorCode::CORRUPTED_FILE: return "CORRUPTED_FILE";
        case ErrorCode::INTERNAL_ERROR: return "INTERNAL_ERROR";
        case ErrorCode::UNKNOWN_ERROR: return "UNKNOWN_ERROR";
        default: return "UNKNOWN";
    }
}

} // namespace utec