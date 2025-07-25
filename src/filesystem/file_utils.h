// src/filesystem/file_utils.h
#pragma once
#include "utils/types.h"
#include <string>
#include <vector>

namespace utec {

    class FileUtils {
    public:
        static bool exists(const std::string& path);
        static bool isDirectory(const std::string& path);
        static bool isVideoFile(const std::string& filename);
        static size_t getFileSize(const std::string& path);
        static std::vector<std::string> listDirectory(const std::string& path);
        static std::string getAbsolutePath(const std::string& path);
        static std::string normalizePath(const std::string& path);

    private:
        static const std::vector<std::string> VIDEO_EXTENSIONS;
    };

} // namespace utec

// ================================================================
