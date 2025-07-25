// src/security/path_validator.h
#pragma once
#include <string>
#include <vector>
#include <set>

namespace utec {

    class PathValidator {
    public:
        // Path validation
        static bool isValidPath(const std::string& path, const std::string& root);
        static bool isWithinRoot(const std::string& path, const std::string& root);
        static std::string sanitizePath(const std::string& path);
        static bool containsPathTraversal(const std::string& path);

        // File extension validation
        static bool isAllowedExtension(const std::string& extension);
        static bool isAllowedExtension(const std::string& extension, const std::vector<std::string>& allowed);
        static std::string extractExtension(const std::string& filename);

        // Security checks
        static bool isSuspiciousPath(const std::string& path);
        static bool containsNullBytes(const std::string& path);
        static bool isValidFilename(const std::string& filename);

        // URL validation
        static std::string validateAndDecodeUrl(const std::string& url_path);
        static bool isValidUrlPath(const std::string& path);

    private:
        static const std::set<std::string> SUSPICIOUS_PATTERNS;
        static const std::set<std::string> FORBIDDEN_NAMES;
        static const std::vector<std::string> DEFAULT_ALLOWED_EXTENSIONS;

        static std::string normalizePath(const std::string& path);
        static std::vector<std::string> splitPath(const std::string& path);
        static bool containsForbiddenName(const std::string& filename);
    };

} // namespace utec