// src/utils/types.h
#pragma once
#include <string>
#include <vector>
#include <map>

namespace utec {

struct VideoFile {
    std::string name;
    std::string path;
    std::string relative_path;
    size_t size;
    std::string extension;
};

struct Course {
    std::string name;
    std::string path;
    std::vector<VideoFile> videos;
};

struct Semester {
    std::string name;
    std::string path;
    std::vector<Course> courses;
};

struct AcademicYear {
    std::string year;
    std::string path;
    std::vector<Semester> semesters;
};

using VideoLibrary = std::vector<AcademicYear>;

} // namespace utec

// ================================================================

// src/utils/logger.h
#pragma once
#include <string>
#include <iostream>
#include <chrono>
#include <iomanip>

namespace utec {

enum class LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
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

// ================================================================

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

// src/filesystem/directory_scanner.h
#pragma once
#include "utils/types.h"
#include <string>

namespace utec {

class DirectoryScanner {
public:
    explicit DirectoryScanner(const std::string& root_path);

    VideoLibrary scanLibrary();
    bool isValidStructure() const;

private:
    std::string root_path_;

    AcademicYear scanYear(const std::string& year_path);
    Semester scanSemester(const std::string& semester_path);
    Course scanCourse(const std::string& course_path);
    std::vector<VideoFile> scanVideos(const std::string& course_path);
};

} // namespace utec