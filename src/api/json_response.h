// src/api/json_response.h
#pragma once
#include "utils/types.h"
#include <string>
#include <map>

namespace utec {

    class JsonResponse {
    public:
        static std::string createLibraryResponse(const VideoLibrary& library);
        static std::string createCourseResponse(const Course& course);
        static std::string createVideoResponse(const VideoFile& video);
        static std::string createErrorResponse(const std::string& error, int code = 500);
        static std::string createSuccessResponse(const std::string& message);

        // Made public to allow access from other classes
        static std::string escapeJson(const std::string& str);
        static std::string vectorToJson(const std::vector<std::string>& vec);
    };

} // namespace utec