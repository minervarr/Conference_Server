// src/security/path_validator.cpp
#include "security/path_validator.h"
#include "utils/string_utils.h"
#include "filesystem/file_utils.h"
#include <algorithm>
#include <regex>
#include <filesystem>

namespace fs = std::filesystem;

namespace utec {

const std::set<std::string> PathValidator::SUSPICIOUS_PATTERNS = {
    "..", "~", "$", "%", "&", "*", "?", "<", ">", "|", "\"", "'"
};

const std::set<std::string> PathValidator::FORBIDDEN_NAMES = {
    "con", "prn", "aux", "nul", "com1", "com2", "com3", "com4", "com5",
    "com6", "com7", "com8", "com9", "lpt1", "lpt2", "lpt3", "lpt4",
    "lpt5", "lpt6", "lpt7", "lpt8", "lpt9"
};

const std::vector<std::string> PathValidator::DEFAULT_ALLOWED_EXTENSIONS = {
    "mp4", "avi", "mkv", "mov", "wmv", "flv", "webm", "m4v", "3gp", "mpg", "mpeg"
};

bool PathValidator::isValidPath(const std::string& path, const std::string& root) {
    if (path.empty() || root.empty()) {
        return false;
    }

    // Check for null bytes
    if (containsNullBytes(path)) {
        return false;
    }

    // Check for path traversal attempts
    if (containsPathTraversal(path)) {
        return false;
    }

    // Check if path is within root
    if (!isWithinRoot(path, root)) {
        return false;
    }

    // Check for suspicious patterns
    if (isSuspiciousPath(path)) {
        return false;
    }

    return true;
}

bool PathValidator::isWithinRoot(const std::string& path, const std::string& root) {
    try {
        std::string normalized_path = FileUtils::getAbsolutePath(path);
        std::string normalized_root = FileUtils::getAbsolutePath(root);

        // Ensure both paths end without trailing slashes for consistent comparison
        if (normalized_path.back() == '/' || normalized_path.back() == '\\') {
            normalized_path.pop_back();
        }
        if (normalized_root.back() == '/' || normalized_root.back() == '\\') {
            normalized_root.pop_back();
        }

        // Check if path starts with root
        return normalized_path.find(normalized_root) == 0 &&
               (normalized_path.length() == normalized_root.length() ||
                normalized_path[normalized_root.length()] == '/' ||
                normalized_path[normalized_root.length()] == '\\');

    } catch (const std::exception&) {
        return false;
    }
}

std::string PathValidator::sanitizePath(const std::string& path) {
    std::string sanitized = path;

    // Remove null bytes
    sanitized.erase(std::remove(sanitized.begin(), sanitized.end(), '\0'), sanitized.end());

    // Replace backslashes with forward slashes for consistency
    std::replace(sanitized.begin(), sanitized.end(), '\\', '/');

    // Remove duplicate slashes
    std::regex multiple_slashes("/+");
    sanitized = std::regex_replace(sanitized, multiple_slashes, "/");

    // Remove leading slash if present
    if (!sanitized.empty() && sanitized[0] == '/') {
        sanitized = sanitized.substr(1);
    }

    // Remove trailing slash if present
    if (!sanitized.empty() && sanitized.back() == '/') {
        sanitized.pop_back();
    }

    return sanitized;
}

bool PathValidator::containsPathTraversal(const std::string& path) {
    // Check for various path traversal patterns
    std::vector<std::string> dangerous_patterns = {
        "..", "..\\", "../", "..%2f", "..%2F", "..%5c", "..%5C",
        "%2e%2e", "%2E%2E", ".%2e", ".%2E", "%2e.", "%2E."
    };

    std::string lower_path = StringUtils::toLower(path);

    for (const auto& pattern : dangerous_patterns) {
        if (lower_path.find(pattern) != std::string::npos) {
            return true;
        }
    }

    return false;
}

bool PathValidator::isAllowedExtension(const std::string& extension) {
    return isAllowedExtension(extension, DEFAULT_ALLOWED_EXTENSIONS);
}

bool PathValidator::isAllowedExtension(const std::string& extension, const std::vector<std::string>& allowed) {
    std::string lower_ext = StringUtils::toLower(extension);

    return std::find(allowed.begin(), allowed.end(), lower_ext) != allowed.end();
}

std::string PathValidator::extractExtension(const std::string& filename) {
    return StringUtils::getFileExtension(filename);
}

bool PathValidator::isSuspiciousPath(const std::string& path) {
    std::string lower_path = StringUtils::toLower(path);

    // Check for suspicious patterns
    for (const auto& pattern : SUSPICIOUS_PATTERNS) {
        if (lower_path.find(pattern) != std::string::npos) {
            return true;
        }
    }

    // Check for control characters
    for (char c : path) {
        if (c < 32 && c != '\t' && c != '\n' && c != '\r') {
            return true;
        }
    }

    // Check for extremely long paths
    if (path.length() > 4096) {
        return true;
    }

    return false;
}

bool PathValidator::containsNullBytes(const std::string& path) {
    return path.find('\0') != std::string::npos;
}

bool PathValidator::isValidFilename(const std::string& filename) {
    if (filename.empty() || filename.length() > 255) {
        return false;
    }

    // Check for forbidden characters
    std::string forbidden_chars = "<>:\"/\\|?*";
    for (char c : forbidden_chars) {
        if (filename.find(c) != std::string::npos) {
            return false;
        }
    }

    // Check for forbidden names (Windows reserved names)
    if (containsForbiddenName(filename)) {
        return false;
    }

    // Check for names that start or end with dot or space
    if (filename.front() == '.' || filename.front() == ' ' ||
        filename.back() == '.' || filename.back() == ' ') {
        return false;
    }

    return true;
}

std::string PathValidator::validateAndDecodeUrl(const std::string& url_path) {
    // URL decode the path
    std::string decoded = StringUtils::urlDecode(url_path);

    // Sanitize the decoded path
    std::string sanitized = sanitizePath(decoded);

    // Additional validation
    if (!isValidUrlPath(sanitized)) {
        return "";
    }

    return sanitized;
}

bool PathValidator::isValidUrlPath(const std::string& path) {
    // Check for empty path
    if (path.empty()) {
        return false;
    }

    // Check for path traversal
    if (containsPathTraversal(path)) {
        return false;
    }

    // Check for suspicious patterns
    if (isSuspiciousPath(path)) {
        return false;
    }

    // Split path and validate each component
    auto components = splitPath(path);
    for (const auto& component : components) {
        if (!isValidFilename(component)) {
            return false;
        }
    }

    return true;
}

std::string PathValidator::normalizePath(const std::string& path) {
    try {
        return fs::path(path).lexically_normal().string();
    } catch (const std::exception&) {
        return path;
    }
}

std::vector<std::string> PathValidator::splitPath(const std::string& path) {
    return StringUtils::split(path, '/');
}

bool PathValidator::containsForbiddenName(const std::string& filename) {
    // Extract base name without extension for Windows reserved name check
    std::string base_name = filename;
    auto dot_pos = base_name.find_last_of('.');
    if (dot_pos != std::string::npos) {
        base_name = base_name.substr(0, dot_pos);
    }

    std::string lower_name = StringUtils::toLower(base_name);

    return FORBIDDEN_NAMES.find(lower_name) != FORBIDDEN_NAMES.end();
}