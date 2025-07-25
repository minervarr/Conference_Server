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

private:
    static std::string escapeJson(const std::string& str);
    static std::string vectorToJson(const std::vector<std::string>& vec);
};

} // namespace utec

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

// src/web/embedded_resources.h
#pragma once
#include <string>

namespace utec {

class EmbeddedResources {
public:
    static std::string getIndexHtml();
    static std::string getMainCss();
    static std::string getMainJs();
    static std::string getFavicon();

    // Template variables replacement
    static std::string processTemplate(const std::string& html,
                                     const std::string& server_url);

private:
    static const std::string INDEX_HTML;
    static const std::string MAIN_CSS;
    static const std::string MAIN_JS;
};

} // namespace utec

// ================================================================

// src/web/template_engine.h
#pragma once
#include <string>
#include <map>

namespace utec {

class TemplateEngine {
public:
    static std::string render(const std::string& template_str,
                             const std::map<std::string, std::string>& variables);

private:
    static void replaceVariable(std::string& content,
                               const std::string& var_name,
                               const std::string& value);
};

} // namespace utec

// ================================================================

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

// src/server/http_server.h
#pragma once
#include <string>
#include <memory>
#include <thread>

// Forward declaration
namespace httplib {
    class Server;
}

namespace utec {

class DirectoryScanner;
class VideoApi;
class RouteHandler;

class HttpServer {
public:
    explicit HttpServer(const std::string& root_path, int port = 8080);
    ~HttpServer();

    bool start();
    void stop();
    bool isRunning() const;

    int getPort() const { return port_; }

private:
    std::string root_path_;
    int port_;
    bool running_;

    std::shared_ptr<DirectoryScanner> scanner_;
    std::shared_ptr<VideoApi> api_;
    std::shared_ptr<RouteHandler> routes_;

    std::unique_ptr<httplib::Server> server_;
    std::thread server_thread_;

    void setupRoutes();
    void printStartupInfo();
};

} // namespace utec