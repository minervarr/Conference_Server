// src/server/route_handler.h
#pragma once
#include <string>
#include <functional>
#include <map>

// Forward declaration for httplib
namespace httplib {
    class Request;
    class Response;
}

namespace utec {

    class VideoApi;

    class RouteHandler {
    public:
        explicit RouteHandler(std::shared_ptr<VideoApi> api, const std::string& root_path);

        // Route handlers
        void handleIndex(const httplib::Request& req, httplib::Response& res);
        void handleLibrary(const httplib::Request& req, httplib::Response& res);
        void handleVideo(const httplib::Request& req, httplib::Response& res);
        void handleVideoStream(const httplib::Request& req, httplib::Response& res);
        void handleStatic(const httplib::Request& req, httplib::Response& res);

        // Utility methods
        std::string getServerUrl(const httplib::Request& req);

    private:
        std::shared_ptr<VideoApi> api_;
        std::string root_path_;

        void setCorsHeaders(httplib::Response& res);
        void setVideoHeaders(httplib::Response& res, const std::string& filename);
        std::string getMimeType(const std::string& extension);
    };

} // namespace utec

// ================================================================
