// src/server/http_server.cpp
#include "server/http_server.h"
#include "server/route_handler.h"
#include "filesystem/directory_scanner.h"
#include "api/video_api.h"
#include "utils/logger.h"
#include "httplib.h"
#include <thread>
#include <memory>

namespace utec {

HttpServer::HttpServer(const std::string& root_path, int port)
    : root_path_(root_path), port_(port), running_(false) {

    // Initialize components
    scanner_ = std::make_shared<DirectoryScanner>(root_path_);
    api_ = std::make_shared<VideoApi>(scanner_);
    routes_ = std::make_shared<RouteHandler>(api_, root_path_);

    server_ = std::make_unique<httplib::Server>();
}

HttpServer::~HttpServer() {
    stop();
}

bool HttpServer::start() {
    if (running_) {
        Logger::warning("Server is already running");
        return false;
    }

    if (!scanner_->isValidStructure()) {
        Logger::error("Invalid directory structure. Expected: root/YYYY/Semester_N/Course_Name/videos");
        return false;
    }

    setupRoutes();

    Logger::info("Starting HTTP server on port " + std::to_string(port_));

    // Start server in a separate thread
    server_thread_ = std::thread([this]() {
        if (!server_->listen("0.0.0.0", port_)) {
            Logger::error("Failed to bind to port " + std::to_string(port_));
            running_ = false;
        }
    });

    // Give the server a moment to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    if (server_->is_running()) {
        running_ = true;
        printStartupInfo();
        return true;
    }

    if (server_thread_.joinable()) {
        server_thread_.join();
    }

    return false;
}

void HttpServer::stop() {
    if (!running_) return;

    Logger::info("Stopping HTTP server...");
    running_ = false;

    if (server_) {
        server_->stop();
    }

    if (server_thread_.joinable()) {
        server_thread_.join();
    }

    Logger::info("HTTP server stopped");
}

bool HttpServer::isRunning() const {
    return running_ && server_ && server_->is_running();
}

void HttpServer::setupRoutes() {
    auto& server = *server_;

    // Static files and frontend
    server.Get("/", [this](const httplib::Request& req, httplib::Response& res) {
        routes_->handleIndex(req, res);
    });

    server.Get("/favicon.ico", [this](const httplib::Request& req, httplib::Response& res) {
        routes_->handleStatic(req, res);
    });

    server.Get("/static/(.*)", [this](const httplib::Request& req, httplib::Response& res) {
        routes_->handleStatic(req, res);
    });

    // API endpoints
    server.Get("/api/library", [this](const httplib::Request& req, httplib::Response& res) {
        routes_->handleLibrary(req, res);
    });

    server.Get("/api/video", [this](const httplib::Request& req, httplib::Response& res) {
        routes_->handleVideo(req, res);
    });

    // Video streaming
    server.Get("/stream/(.*)", [this](const httplib::Request& req, httplib::Response& res) {
        routes_->handleVideoStream(req, res);
    });

    // Enable CORS for all routes
    server.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        return httplib::Server::HandlerResponse::Unhandled;
    });

    // Handle OPTIONS requests for CORS
    server.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        return;
    });

    // Set server timeouts
    server.set_read_timeout(300);  // 5 minutes for large video files
    server.set_write_timeout(300);
}

void HttpServer::printStartupInfo() {
    Logger::info("========================================");
    Logger::info("UTEC Conference Server is running!");
    Logger::info("========================================");
    Logger::info("Local access: http://localhost:" + std::to_string(port_));
    Logger::info("Network access: http://[your-ip]:" + std::to_string(port_));
    Logger::info("Serving videos from: " + root_path_);
    Logger::info("========================================");
    Logger::info("To stop the server, press Ctrl+C");
}

} // namespace utec

// ================================================================

// src/server/route_handler.cpp
#include "server/route_handler.h"
#include "api/video_api.h"
#include "web/embedded_resources.h"
#include "filesystem/file_utils.h"
#include "utils/string_utils.h"
#include "utils/logger.h"
#include "httplib.h"
#include <fstream>
#include <map>

namespace utec {

RouteHandler::RouteHandler(std::shared_ptr<VideoApi> api, const std::string& root_path)
    : api_(api), root_path_(root_path) {
}

void RouteHandler::handleIndex(const httplib::Request& req, httplib::Response& res) {
    Logger::debug("Serving index page");

    std::string server_url = getServerUrl(req);
    std::string html = EmbeddedResources::processTemplate(
        EmbeddedResources::getIndexHtml(),
        server_url
    );

    res.set_content(html, "text/html; charset=utf-8");
}

void RouteHandler::handleLibrary(const httplib::Request& req, httplib::Response& res) {
    Logger::debug("API: Getting video library");

    setCorsHeaders(res);

    try {
        std::string json_response = api_->getLibrary();
        res.set_content(json_response, "application/json; charset=utf-8");
    } catch (const std::exception& e) {
        Logger::error("Error getting library: " + std::string(e.what()));
        res.status = 500;
        res.set_content("{\"error\":\"Internal server error\"}", "application/json");
    }
}

void RouteHandler::handleVideo(const httplib::Request& req, httplib::Response& res) {
    setCorsHeaders(res);

    // Parse query parameters
    auto year = req.get_param_value("year");
    auto semester = req.get_param_value("semester");
    auto course = req.get_param_value("course");
    auto video = req.get_param_value("video");

    if (year.empty() || semester.empty() || course.empty() || video.empty()) {
        res.status = 400;
        res.set_content("{\"error\":\"Missing required parameters\"}", "application/json");
        return;
    }

    Logger::debug("API: Getting video info for " + video);

    try {
        std::string json_response = api_->getVideo(year, semester, course, video);
        res.set_content(json_response, "application/json; charset=utf-8");
    } catch (const std::exception& e) {
        Logger::error("Error getting video: " + std::string(e.what()));
        res.status = 404;
        res.set_content("{\"error\":\"Video not found\"}", "application/json");
    }
}

void RouteHandler::handleVideoStream(const httplib::Request& req, httplib::Response& res) {
    std::string relative_path = req.matches[1];
    relative_path = StringUtils::urlDecode(relative_path);

    std::string full_path = root_path_ + "/" + relative_path;
    full_path = FileUtils::normalizePath(full_path);

    Logger::info("Streaming video: " + relative_path);

    // Security check: ensure the path is within our root directory
    std::string normalized_root = FileUtils::normalizePath(root_path_);
    if (full_path.find(normalized_root) != 0) {
        Logger::warning("Attempted access outside root directory: " + full_path);
        res.status = 403;
        res.set_content("Forbidden", "text/plain");
        return;
    }

    if (!FileUtils::exists(full_path) || !FileUtils::isVideoFile(full_path)) {
        Logger::warning("Video file not found: " + full_path);
        res.status = 404;
        res.set_content("Video not found", "text/plain");
        return;
    }

    // Set video headers
    setVideoHeaders(res, StringUtils::getBaseName(full_path));

    // Stream the file
    std::ifstream file(full_path, std::ios::binary);
    if (!file) {
        Logger::error("Failed to open video file: " + full_path);
        res.status = 500;
        res.set_content("Internal server error", "text/plain");
        return;
    }

    // Get file size
    size_t file_size = FileUtils::getFileSize(full_path);

    // Handle range requests for video seeking
    auto range_header = req.get_header_value("Range");
    if (!range_header.empty() && range_header.find("bytes=") == 0) {
        // Parse range header (simplified implementation)
        std::string range_spec = range_header.substr(6); // Remove "bytes="
        auto dash_pos = range_spec.find('-');

        if (dash_pos != std::string::npos) {
            size_t start = 0, end = file_size - 1;

            std::string start_str = range_spec.substr(0, dash_pos);
            std::string end_str = range_spec.substr(dash_pos + 1);

            if (!start_str.empty()) start = std::stoull(start_str);
            if (!end_str.empty()) end = std::stoull(end_str);

            if (start <= end && end < file_size) {
                size_t content_length = end - start + 1;

                res.status = 206; // Partial Content
                res.set_header("Content-Range",
                    "bytes " + std::to_string(start) + "-" + std::to_string(end) +
                    "/" + std::to_string(file_size));
                res.set_header("Content-Length", std::to_string(content_length));

                file.seekg(start);

                std::vector<char> buffer(std::min(content_length, size_t(64 * 1024))); // 64KB chunks
                std::string content;
                content.reserve(content_length);

                size_t remaining = content_length;
                while (remaining > 0 && file) {
                    size_t to_read = std::min(remaining, buffer.size());
                    file.read(buffer.data(), to_read);
                    size_t actually_read = file.gcount();

                    content.append(buffer.data(), actually_read);
                    remaining -= actually_read;

                    if (actually_read < to_read) break;
                }

                res.set_content(content, getMimeType(StringUtils::getFileExtension(full_path)));
                return;
            }
        }
    }

    // Full file response
    res.set_header("Content-Length", std::to_string(file_size));

    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());

    res.set_content(content, getMimeType(StringUtils::getFileExtension(full_path)));
}

void RouteHandler::handleStatic(const httplib::Request& req, httplib::Response& res) {
    std::string path = req.path;

    if (path == "/favicon.ico") {
        res.set_content(EmbeddedResources::getFavicon(), "image/x-icon");
    } else if (path.find("/static/") == 0) {
        std::string filename = path.substr(8); // Remove "/static/"

        if (filename == "style.css") {
            res.set_content(EmbeddedResources::getMainCss(), "text/css");
        } else if (filename == "app.js") {
            res.set_content(EmbeddedResources::getMainJs(), "application/javascript");
        } else {
            res.status = 404;
            res.set_content("Not found", "text/plain");
        }
    } else {
        res.status = 404;
        res.set_content("Not found", "text/plain");
    }
}

std::string RouteHandler::getServerUrl(const httplib::Request& req) {
    std::string host = req.get_header_value("Host");
    if (host.empty()) {
        host = "localhost:" + std::to_string(8080); // fallback
    }
    return "http://" + host;
}

void RouteHandler::setCorsHeaders(httplib::Response& res) {
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

void RouteHandler::setVideoHeaders(httplib::Response& res, const std::string& filename) {
    res.set_header("Accept-Ranges", "bytes");
    res.set_header("Cache-Control", "no-cache");
    res.set_header("Content-Disposition", "inline; filename=\"" + filename + "\"");
}

std::string RouteHandler::getMimeType(const std::string& extension) {
    static const std::map<std::string, std::string> mime_types = {
        {"mp4", "video/mp4"},
        {"avi", "video/x-msvideo"},
        {"mkv", "video/x-matroska"},
        {"mov", "video/quicktime"},
        {"wmv", "video/x-ms-wmv"},
        {"flv", "video/x-flv"},
        {"webm", "video/webm"},
        {"m4v", "video/x-m4v"},
        {"3gp", "video/3gpp"},
        {"mpg", "video/mpeg"},
        {"mpeg", "video/mpeg"}
    };

    auto it = mime_types.find(extension);
    return (it != mime_types.end()) ? it->second : "application/octet-stream";
}

} // namespace utec