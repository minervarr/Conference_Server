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

// ================================================================

// src/api/video_api.cpp
#include "api/video_api.h"
#include "api/json_response.h"
#include "filesystem/directory_scanner.h"
#include "utils/logger.h"
#include "utils/string_utils.h"

namespace utec {

VideoApi::VideoApi(std::shared_ptr<DirectoryScanner> scanner)
    : scanner_(scanner), cache_valid_(false) {
}

std::string VideoApi::getLibrary() {
    refreshCache();
    return JsonResponse::createLibraryResponse(cached_library_);
}

std::string VideoApi::getCourse(const std::string& year, const std::string& semester, const std::string& course) {
    refreshCache();

    for (const auto& y : cached_library_) {
        if (y.year == year) {
            for (const auto& s : y.semesters) {
                if (s.name == semester) {
                    for (const auto& c : s.courses) {
                        if (c.name == course) {
                            return JsonResponse::createCourseResponse(c);
                        }
                    }
                }
            }
        }
    }

    return JsonResponse::createErrorResponse("Course not found", 404);
}

std::string VideoApi::getVideo(const std::string& year, const std::string& semester,
                              const std::string& course, const std::string& video) {
    refreshCache();

    VideoFile* found_video = findVideo(year, semester, course, video);
    if (found_video) {
        return JsonResponse::createVideoResponse(*found_video);
    }

    return JsonResponse::createErrorResponse("Video not found", 404);
}

std::string VideoApi::searchVideos(const std::string& query) {
    refreshCache();

    std::string lower_query = StringUtils::toLower(query);
    std::vector<VideoFile> results;

    for (const auto& year : cached_library_) {
        for (const auto& semester : year.semesters) {
            for (const auto& course : semester.courses) {
                for (const auto& video : course.videos) {
                    std::string lower_name = StringUtils::toLower(video.name);
                    if (lower_name.find(lower_query) != std::string::npos) {
                        results.push_back(video);
                    }
                }
            }
        }
    }

    // Create a simple JSON response for search results
    std::ostringstream json;
    json << "{\n  \"status\": \"success\",\n  \"data\": {\n    \"query\": \""
         << JsonResponse::escapeJson(query) << "\",\n    \"results\": [\n";

    for (size_t i = 0; i < results.size(); ++i) {
        const auto& video = results[i];
        json << "      {\n";
        json << "        \"name\": \"" << JsonResponse::escapeJson(video.name) << "\",\n";
        json << "        \"path\": \"" << JsonResponse::escapeJson(video.relative_path) << "\"\n";
        json << "      }";
        if (i < results.size() - 1) json << ",";
        json << "\n";
    }

    json << "    ]\n  }\n}";
    return json.str();
}

void VideoApi::refreshCache() {
    if (!cache_valid_) {
        Logger::debug("Refreshing video library cache");
        cached_library_ = scanner_->scanLibrary();
        cache_valid_ = true;
    }
}

VideoFile* VideoApi::findVideo(const std::string& year, const std::string& semester,
                              const std::string& course, const std::string& video) {
    for (auto& y : cached_library_) {
        if (y.year == year) {
            for (auto& s : y.semesters) {
                if (s.name == semester) {
                    for (auto& c : s.courses) {
                        if (c.name == course) {
                            for (auto& v : c.videos) {
                                if (v.name == video) {
                                    return &v;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return nullptr;
}

} // namespace utec