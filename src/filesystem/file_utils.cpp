// src/filesystem/file_utils.cpp
#include "filesystem/file_utils.h"
#include "utils/string_utils.h"
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

namespace utec {

const std::vector<std::string> FileUtils::VIDEO_EXTENSIONS = {
    "mp4", "avi", "mkv", "mov", "wmv", "flv", "webm", "m4v", "3gp", "mpg", "mpeg"
};

bool FileUtils::exists(const std::string& path) {
    return fs::exists(path);
}

bool FileUtils::isDirectory(const std::string& path) {
    return fs::is_directory(path);
}

bool FileUtils::isVideoFile(const std::string& filename) {
    std::string extension = StringUtils::getFileExtension(filename);
    return std::find(VIDEO_EXTENSIONS.begin(), VIDEO_EXTENSIONS.end(), extension)
           != VIDEO_EXTENSIONS.end();
}

size_t FileUtils::getFileSize(const std::string& path) {
    try {
        return fs::file_size(path);
    } catch (const fs::filesystem_error&) {
        return 0;
    }
}

std::vector<std::string> FileUtils::listDirectory(const std::string& path) {
    std::vector<std::string> entries;

    try {
        for (const auto& entry : fs::directory_iterator(path)) {
            entries.push_back(entry.path().filename().string());
        }

        // Sort entries for consistent ordering
        std::sort(entries.begin(), entries.end());
    } catch (const fs::filesystem_error&) {
        // Return empty vector on error
    }

    return entries;
}

std::string FileUtils::getAbsolutePath(const std::string& path) {
    try {
        return fs::absolute(path).string();
    } catch (const fs::filesystem_error&) {
        return path;
    }
}

std::string FileUtils::normalizePath(const std::string& path) {
    try {
        return fs::path(path).lexically_normal().string();
    } catch (const fs::filesystem_error&) {
        return path;
    }
}

} // namespace utec

