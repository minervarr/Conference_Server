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

// ================================================================

// src/filesystem/directory_scanner.cpp
#include "filesystem/directory_scanner.h"
#include "filesystem/file_utils.h"
#include "utils/string_utils.h"
#include "utils/logger.h"
#include <filesystem>

namespace fs = std::filesystem;

namespace utec {

DirectoryScanner::DirectoryScanner(const std::string& root_path)
    : root_path_(FileUtils::getAbsolutePath(root_path)) {
    Logger::info("Initializing directory scanner for: " + root_path_);
}

VideoLibrary DirectoryScanner::scanLibrary() {
    VideoLibrary library;

    if (!FileUtils::exists(root_path_) || !FileUtils::isDirectory(root_path_)) {
        Logger::error("Root path does not exist or is not a directory: " + root_path_);
        return library;
    }

    Logger::info("Scanning video library...");

    auto entries = FileUtils::listDirectory(root_path_);
    for (const auto& entry : entries) {
        std::string year_path = root_path_ + "/" + entry;

        if (FileUtils::isDirectory(year_path)) {
            // Check if it looks like a year (4 digits)
            if (entry.length() == 4 && std::all_of(entry.begin(), entry.end(), ::isdigit)) {
                Logger::debug("Scanning year: " + entry);
                AcademicYear year = scanYear(year_path);
                if (!year.semesters.empty()) {
                    library.push_back(year);
                }
            }
        }
    }

    Logger::info("Library scan complete. Found " + std::to_string(library.size()) + " years");
    return library;
}

bool DirectoryScanner::isValidStructure() const {
    if (!FileUtils::exists(root_path_) || !FileUtils::isDirectory(root_path_)) {
        return false;
    }

    // Check if there's at least one year directory
    auto entries = FileUtils::listDirectory(root_path_);
    for (const auto& entry : entries) {
        if (entry.length() == 4 && std::all_of(entry.begin(), entry.end(), ::isdigit)) {
            std::string year_path = root_path_ + "/" + entry;
            if (FileUtils::isDirectory(year_path)) {
                return true;
            }
        }
    }

    return false;
}

AcademicYear DirectoryScanner::scanYear(const std::string& year_path) {
    AcademicYear year;
    year.path = year_path;
    year.year = StringUtils::getBaseName(year_path);

    auto entries = FileUtils::listDirectory(year_path);
    for (const auto& entry : entries) {
        std::string semester_path = year_path + "/" + entry;

        if (FileUtils::isDirectory(semester_path)) {
            // Check if it looks like a semester (starts with "Semester_")
            if (entry.find("Semester_") == 0) {
                Logger::debug("Scanning semester: " + entry);
                Semester semester = scanSemester(semester_path);
                if (!semester.courses.empty()) {
                    year.semesters.push_back(semester);
                }
            }
        }
    }

    return year;
}

Semester DirectoryScanner::scanSemester(const std::string& semester_path) {
    Semester semester;
    semester.path = semester_path;
    semester.name = StringUtils::getBaseName(semester_path);

    auto entries = FileUtils::listDirectory(semester_path);
    for (const auto& entry : entries) {
        std::string course_path = semester_path + "/" + entry;

        if (FileUtils::isDirectory(course_path)) {
            Logger::debug("Scanning course: " + entry);
            Course course = scanCourse(course_path);
            if (!course.videos.empty()) {
                semester.courses.push_back(course);
            }
        }
    }

    return semester;
}

Course DirectoryScanner::scanCourse(const std::string& course_path) {
    Course course;
    course.path = course_path;
    course.name = StringUtils::getBaseName(course_path);

    // Replace underscores with spaces for display
    course.name = StringUtils::replaceAll(course.name, "_", " ");

    course.videos = scanVideos(course_path);

    Logger::debug("Found " + std::to_string(course.videos.size()) + " videos in course: " + course.name);
    return course;
}

std::vector<VideoFile> DirectoryScanner::scanVideos(const std::string& course_path) {
    std::vector<VideoFile> videos;

    auto entries = FileUtils::listDirectory(course_path);
    for (const auto& entry : entries) {
        std::string file_path = course_path + "/" + entry;

        if (!FileUtils::isDirectory(file_path) && FileUtils::isVideoFile(entry)) {
            VideoFile video;
            video.name = entry;
            video.path = file_path;

            // Create relative path from root
            std::string relative = file_path;
            if (relative.find(root_path_) == 0) {
                relative = relative.substr(root_path_.length());
                if (relative[0] == '/' || relative[0] == '\\') {
                    relative = relative.substr(1);
                }
            }
            video.relative_path = relative;

            video.size = FileUtils::getFileSize(file_path);
            video.extension = StringUtils::getFileExtension(entry);

            videos.push_back(video);
        }
    }

    return videos;
}

} // namespace utec