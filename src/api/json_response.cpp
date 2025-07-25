// src/api/json_response.cpp
#include "api/json_response.h"
#include "utils/string_utils.h"
#include <sstream>

namespace utec {

std::string JsonResponse::createLibraryResponse(const VideoLibrary& library) {
    std::ostringstream json;
    json << "{\n  \"status\": \"success\",\n  \"data\": {\n    \"years\": [\n";

    for (size_t i = 0; i < library.size(); ++i) {
        const auto& year = library[i];
        json << "      {\n";
        json << "        \"year\": \"" << escapeJson(year.year) << "\",\n";
        json << "        \"semesters\": [\n";

        for (size_t j = 0; j < year.semesters.size(); ++j) {
            const auto& semester = year.semesters[j];
            json << "          {\n";
            json << "            \"name\": \"" << escapeJson(semester.name) << "\",\n";
            json << "            \"courses\": [\n";

            for (size_t k = 0; k < semester.courses.size(); ++k) {
                const auto& course = semester.courses[k];
                json << "              {\n";
                json << "              {\n";
                json << "                \"name\": \"" << escapeJson(course.name) << "\",\n";
                json << "                \"video_count\": " << course.videos.size() << "\n";
                json << "              }";
                if (k < semester.courses.size() - 1) json << ",";
                json << "\n";
            }

            json << "            ]\n";
            json << "          }";
            if (j < year.semesters.size() - 1) json << ",";
            json << "\n";
        }

        json << "        ]\n";
        json << "      }";
        if (i < library.size() - 1) json << ",";
        json << "\n";
    }

    json << "    ]\n  }\n}";
    return json.str();
}

std::string JsonResponse::createCourseResponse(const Course& course) {
    std::ostringstream json;
    json << "{\n  \"status\": \"success\",\n  \"data\": {\n";
    json << "    \"name\": \"" << escapeJson(course.name) << "\",\n";
    json << "    \"videos\": [\n";

    for (size_t i = 0; i < course.videos.size(); ++i) {
        const auto& video = course.videos[i];
        json << "      {\n";
        json << "        \"name\": \"" << escapeJson(video.name) << "\",\n";
        json << "        \"path\": \"" << escapeJson(video.relative_path) << "\",\n";
        json << "        \"size\": " << video.size << ",\n";
        json << "        \"extension\": \"" << escapeJson(video.extension) << "\"\n";
        json << "      }";
        if (i < course.videos.size() - 1) json << ",";
        json << "\n";
    }

    json << "    ]\n  }\n}";
    return json.str();
}

std::string JsonResponse::createVideoResponse(const VideoFile& video) {
    std::ostringstream json;
    json << "{\n  \"status\": \"success\",\n  \"data\": {\n";
    json << "    \"name\": \"" << escapeJson(video.name) << "\",\n";
    json << "    \"path\": \"" << escapeJson(video.relative_path) << "\",\n";
    json << "    \"size\": " << video.size << ",\n";
    json << "    \"extension\": \"" << escapeJson(video.extension) << "\"\n";
    json << "  }\n}";
    return json.str();
}

std::string JsonResponse::createErrorResponse(const std::string& error, int code) {
    std::ostringstream json;
    json << "{\n  \"status\": \"error\",\n";
    json << "  \"code\": " << code << ",\n";
    json << "  \"message\": \"" << escapeJson(error) << "\"\n}";
    return json.str();
}

std::string JsonResponse::createSuccessResponse(const std::string& message) {
    std::ostringstream json;
    json << "{\n  \"status\": \"success\",\n";
    json << "  \"message\": \"" << escapeJson(message) << "\"\n}";
    return json.str();
}

std::string JsonResponse::escapeJson(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '"':  result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:   result += c; break;
        }
    }
    return result;
}

std::string JsonResponse::vectorToJson(const std::vector<std::string>& vec) {
    std::ostringstream json;
    json << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        json << "\"" << escapeJson(vec[i]) << "\"";
        if (i < vec.size() - 1) json << ", ";
    }
    json << "]";
    return json.str();
}

} // namespace utec

