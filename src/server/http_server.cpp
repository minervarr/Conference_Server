// src/server/http_server.cpp
#include "server/http_server.h"
#include "server/route_handler.h"
#include "filesystem/directory_scanner.h"
#include "api/video_api.h"
#include "core/error_handler.h"
#include "utils/logger.h"
#include "httplib.h"
#include <thread>
#include <memory>

namespace utec {

HttpServer::HttpServer(const ServerConfig& config)
    : config_(config), running_(false) {

    if (!validateConfiguration()) {
        throw ServerException(ErrorCode::INVALID_CONFIG, "Invalid server configuration");
    }

    // Initialize components
    scanner_ = std::make_shared<DirectoryScanner>(config_.root_path);
    api_ = std::make_shared<VideoApi>(scanner_);
    routes_ = std::make_shared<RouteHandler>(api_, config_.root_path, config_);

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
        throw ServerException(ErrorCode::DIRECTORY_NOT_FOUND,
            "Invalid directory structure. Expected: root/YYYY/Semester_N/Course_Name/videos",
            config_.root_path);
    }

    setupRoutes();

    Logger::info("Starting HTTP server on " + config_.bind_address + ":" + std::to_string(config_.port));

    // Start server in a separate thread
    server_thread_ = std::thread([this]() {
        try {
            if (!server_->listen(config_.bind_address.c_str(), config_.port)) {
                Logger::error("Failed to bind to " + config_.bind_address + ":" + std::to_string(config_.port));
                running_ = false;
            }
        } catch (const std::exception& e) {
            Logger::error("Server thread error: " + std::string(e.what()));
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

    // Check if the thread failed to start
    if (server_thread_.joinable()) {
        server_thread_.join();
    }

    throw ServerException::serverStartFailed(config_.port, "Failed to bind to port");
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

bool HttpServer::validateConfiguration() {
    if (!config_.isValid()) {
        Logger::error("Invalid configuration: " + config_.getValidationError());
        return false;
    }

    // Additional server-specific validation
    if (config_.port <= 1024 && config_.port != 80 && config_.port != 443) {
        Logger::warning("Using privileged port " + std::to_string(config_.port) +
                       " - ensure you have appropriate permissions");
    }

    return true;
}

void HttpServer::setupRoutes() {
    auto& server = *server_;

    // Set server timeouts
    server.set_read_timeout(config_.read_timeout);
    server.set_write_timeout(config_.write_timeout);

    // Static files and frontend
    server.Get("/", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            routes_->handleIndex(req, res);
        } catch (const ServerException& e) {
            ErrorHandler::logError(e);
            res.status = e.getHttpStatus();
            res.set_content(ErrorHandler::formatErrorResponse(e), "application/json");
        } catch (const std::exception& e) {
            ErrorHandler::logError("handleIndex", e);
            res.status = 500;
            res.set_content(ErrorHandler::formatErrorResponse(ErrorCode::INTERNAL_ERROR,
                "Internal server error"), "application/json");
        }
    });

    server.Get("/favicon.ico", [this](const httplib::Request& req, httplib::Response& res) {
        routes_->handleStatic(req, res);
    });

    server.Get("/static/(.*)", [this](const httplib::Request& req, httplib::Response& res) {
        routes_->handleStatic(req, res);
    });

    // API endpoints with error handling
    server.Get("/api/library", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            routes_->handleLibrary(req, res);
        } catch (const ServerException& e) {
            ErrorHandler::logError(e);
            res.status = e.getHttpStatus();
            res.set_content(ErrorHandler::formatErrorResponse(e), "application/json");
        } catch (const std::exception& e) {
            ErrorHandler::logError("handleLibrary", e);
            res.status = 500;
            res.set_content(ErrorHandler::formatErrorResponse(ErrorCode::INTERNAL_ERROR,
                "Failed to load library"), "application/json");
        }
    });

    server.Get("/api/video", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            routes_->handleVideo(req, res);
        } catch (const ServerException& e) {
            ErrorHandler::logError(e);
            res.status = e.getHttpStatus();
            res.set_content(ErrorHandler::formatErrorResponse(e), "application/json");
        } catch (const std::exception& e) {
            ErrorHandler::logError("handleVideo", e);
            res.status = 500;
            res.set_content(ErrorHandler::formatErrorResponse(ErrorCode::INTERNAL_ERROR,
                "Failed to get video information"), "application/json");
        }
    });

    // Video streaming with enhanced error handling
    server.Get("/stream/(.*)", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            routes_->handleVideoStream(req, res);
        } catch (const ServerException& e) {
            ErrorHandler::logError(e);
            res.status = e.getHttpStatus();

            // For streaming errors, return plain text instead of JSON
            if (e.getCode() == ErrorCode::FILE_NOT_FOUND) {
                res.set_content("Video not found", "text/plain");
            } else if (e.getCode() == ErrorCode::PATH_TRAVERSAL_ATTEMPT) {
                res.set_content("Forbidden", "text/plain");
            } else {
                res.set_content("Internal server error", "text/plain");
            }
        } catch (const std::exception& e) {
            ErrorHandler::logError("handleVideoStream", e);
            res.status = 500;
            res.set_content("Internal server error", "text/plain");
        }
    });

    // Enable CORS if configured
    if (config_.enable_cors) {
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
    }

    // Global error handler
    server.set_error_handler([](const httplib::Request& req, httplib::Response& res) {
        Logger::error("HTTP error " + std::to_string(res.status) + " for " + req.path);

        if (req.path.find("/api/") == 0) {
            // API endpoints should return JSON errors
            res.set_content(ErrorHandler::formatErrorResponse(ErrorCode::RESOURCE_NOT_FOUND,
                "Endpoint not found"), "application/json");
        } else {
            // Other endpoints can return plain text
            res.set_content("Not Found", "text/plain");
        }
    });
}

void HttpServer::printStartupInfo() {
    Logger::info("========================================");
    Logger::info("UTEC Conference Server is running!");
    Logger::info("========================================");
    Logger::info("Local access: http://localhost:" + std::to_string(config_.port));
    Logger::info("Network access: http://[your-ip]:" + std::to_string(config_.port));
    Logger::info("Serving videos from: " + config_.root_path);
    Logger::info("Max file size: " + std::to_string(config_.max_file_size / (1024*1024)) + " MB");
    Logger::info("========================================");
    Logger::info("To stop the server, press Ctrl+C");
}

} // namespace utec