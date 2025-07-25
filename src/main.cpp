// src/main.cpp
#include "server/http_server.h"
#include "config/server_config.h"
#include "core/error_handler.h"
#include "utils/logger.h"
#include <iostream>
#include <csignal>
#include <memory>
#include <thread>
#include <chrono>

using namespace utec;

// Global server instance for signal handling
std::unique_ptr<HttpServer> g_server;

void signalHandler(int signal) {
    Logger::info("Received signal " + std::to_string(signal) + ", shutting down...");
    if (g_server) {
        g_server->stop();
    }
    exit(0);
}

int main(int argc, char* argv[]) {
    try {
        // Load configuration from command line arguments
        ServerConfig config = ConfigManager::loadFromArgs(argc, argv);

        if (!config.isValid()) {
            std::string error = config.getValidationError();
            if (!error.empty()) {
                Logger::error(error);
            }
            return 1;
        }

        // Setup signal handlers
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);
#ifndef _WIN32
        signal(SIGHUP, signalHandler);
#endif

        // Configure logging
        if (config.enable_debug_logging) {
            Logger::info("Debug logging enabled");
        }

        Logger::info("UTEC Conference Server starting...");
        Logger::info("Configuration:");
        Logger::info("  - Recordings path: " + config.root_path);
        Logger::info("  - Server port: " + std::to_string(config.port));
        Logger::info("  - Bind address: " + config.bind_address);
        Logger::info("  - Max file size: " + std::to_string(config.max_file_size / (1024*1024)) + " MB");
        Logger::info("  - Range requests: " + std::string(config.enable_range_requests ? "enabled" : "disabled"));
        Logger::info("  - CORS: " + std::string(config.enable_cors ? "enabled" : "disabled"));

        // Create and start server
        g_server = std::make_unique<HttpServer>(config);

        if (!g_server->start()) {
            throw ServerException::serverStartFailed(config.port, "Failed to bind to port");
        }

        Logger::info("Server started successfully!");
        Logger::info("Access your videos at:");
        Logger::info("  - Local: http://localhost:" + std::to_string(config.port));
        Logger::info("  - Network: http://[your-ip]:" + std::to_string(config.port));
        Logger::info("");
        Logger::info("Press Ctrl+C to stop the server");

        // Keep the main thread alive
        while (g_server->isRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

    } catch (const ServerException& e) {
        ErrorHandler::logError(e);
        Logger::error("Server failed to start: " + e.getMessage());
        if (!e.getDetails().empty()) {
            Logger::error("Details: " + e.getDetails());
        }
        return 1;
    } catch (const std::exception& e) {
        ErrorHandler::logError("main", e);
        Logger::error("Unexpected error: " + std::string(e.what()));
        return 1;
    }

    Logger::info("Server stopped.");
    return 0;
}