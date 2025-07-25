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



