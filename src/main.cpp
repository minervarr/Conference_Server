// src/main.cpp
#include "server/http_server.h"
#include "utils/logger.h"
#include <iostream>
#include <string>
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

void printUsage(const char* program_name) {
    std::cout << "UTEC Conference Server - Local Video Streaming\n";
    std::cout << "Usage: " << program_name << " <recordings_path> [port]\n";
    std::cout << "\n";
    std::cout << "Arguments:\n";
    std::cout << "  recordings_path  Path to your video recordings directory\n";
    std::cout << "  port            Server port (default: 8080)\n";
    std::cout << "\n";
    std::cout << "Example:\n";
    std::cout << "  " << program_name << " /mnt/sharedFiles/UTEC/recordings\n";
    std::cout << "  " << program_name << " /mnt/sharedFiles/UTEC/recordings 9000\n";
    std::cout << "\n";
    std::cout << "Expected directory structure:\n";
    std::cout << "  recordings/\n";
    std::cout << "  └── YYYY/\n";
    std::cout << "      └── Semester_N/\n";
    std::cout << "          └── Course_Name/\n";
    std::cout << "              ├── Week_XX_TYPE.mp4\n";
    std::cout << "              └── ...\n";
}

bool validatePath(const std::string& path) {
    // This will be implemented in file_utils.cpp
    // For now, just check if it looks reasonable
    return !path.empty() && path != "." && path != "..";
}

int main(int argc, char* argv[]) {
    // Parse command line arguments
    if (argc < 2 || argc > 3) {
        printUsage(argv[0]);
        return 1;
    }

    std::string recordings_path = argv[1];
    int port = 8080;

    if (argc == 3) {
        try {
            port = std::stoi(argv[2]);
            if (port < 1 || port > 65535) {
                Logger::error("Port must be between 1 and 65535");
                return 1;
            }
        } catch (const std::exception& e) {
            Logger::error("Invalid port number: " + std::string(argv[2]));
            return 1;
        }
    }

    // Validate recordings path
    if (!validatePath(recordings_path)) {
        Logger::error("Invalid recordings path: " + recordings_path);
        return 1;
    }

    // Setup signal handlers
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
#ifndef _WIN32
    signal(SIGHUP, signalHandler);
#endif

    Logger::info("UTEC Conference Server starting...");
    Logger::info("Recordings path: " + recordings_path);
    Logger::info("Server port: " + std::to_string(port));

    try {
        // Create and start server
        g_server = std::make_unique<HttpServer>(recordings_path, port);

        if (!g_server->start()) {
            Logger::error("Failed to start server");
            return 1;
        }

        Logger::info("Server started successfully!");
        Logger::info("Access your videos at http://localhost:" + std::to_string(port));
        Logger::info("Press Ctrl+C to stop the server");

        // Keep the main thread alive
        while (g_server->isRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

    } catch (const std::exception& e) {
        Logger::error("Server error: " + std::string(e.what()));
        return 1;
    }

    Logger::info("Server stopped.");
    return 0;
}