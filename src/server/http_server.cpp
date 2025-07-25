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

