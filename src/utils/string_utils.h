// src/utils/string_utils.h
#pragma once
#include <string>
#include <vector>

namespace utec {

    class StringUtils {
    public:
        static std::string trim(const std::string& str);
        static std::string toLower(const std::string& str);
        static std::string urlDecode(const std::string& str);
        static std::string urlEncode(const std::string& str);
        static std::vector<std::string> split(const std::string& str, char delimiter);
        static std::string join(const std::vector<std::string>& parts, const std::string& separator);
        static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
        static bool endsWith(const std::string& str, const std::string& suffix);
        static std::string getFileExtension(const std::string& filename);
        static std::string getBaseName(const std::string& path);
    };

} // namespace utec

// ================================================================
