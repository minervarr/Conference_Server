
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