// ================================================================

// src/api/video_api.h
#pragma once
#include "utils/types.h"
#include <string>
#include <memory>

namespace utec {

    class DirectoryScanner;

    class VideoApi {
    public:
        explicit VideoApi(std::shared_ptr<DirectoryScanner> scanner);

        std::string getLibrary();
        std::string getCourse(const std::string& year, const std::string& semester, const std::string& course);
        std::string getVideo(const std::string& year, const std::string& semester,
                            const std::string& course, const std::string& video);
        std::string searchVideos(const std::string& query);

    private:
        std::shared_ptr<DirectoryScanner> scanner_;
        VideoLibrary cached_library_;
        bool cache_valid_;

        void refreshCache();
        VideoFile* findVideo(const std::string& year, const std::string& semester,
                            const std::string& course, const std::string& video);
    };

} // namespace utec

// ================================================================
