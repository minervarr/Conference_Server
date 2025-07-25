// src/config/server_config.cpp
#include "config/server_config.h"
#include "filesystem/file_utils.h"
#include "utils/logger.h"
#include <iostream>
#include <algorithm>

namespace utec {

bool ServerConfig::isValid() const {
    return !root_path.empty() &&
           port > 0 && port <= 65535 &&
           max_file_size > 0 &&
           read_timeout > 0 &&
           write_timeout > 0;
}

std::string ServerConfig::getValidationError() const {
    if (root_path.empty()) {
        return "Root path cannot be empty";
    }
    if (port <= 0 || port > 65535) {
        return "Port must be between 1 and 65535";
    }
    if (max_file_size == 0) {
        return "Max file size must be greater than 0";
    }
    if (read_timeout <= 0 || write_timeout <= 0) {
        return "Timeouts must be greater than 0";
    }
    return "";
}

ServerConfig ConfigManager::createDefault() {
    ServerConfig config;
    // Default values are already set in the struct
    return config;
}

ServerConfig ConfigManager::loadFromArgs(int argc, char* argv[]) {
    ServerConfig config = createDefault();

    // Parse command line arguments
    if (argc < 2 || argc > 4) {
        printUsage(argv[0]);
        config.root_path = ""; // Mark as invalid
        return config;
    }

    // Required: root path
    config.root_path = normalizeRootPath(argv[1]);

    // Optional: port
    if (argc >= 3) {
        try {
            int port = std::stoi(argv[2]);
            if (isValidPort(port)) {
                config.port = port;
            } else {
                Logger::error("Invalid port number: " + std::string(argv[2]));
                config.root_path = ""; // Mark as invalid
                return config;
            }
        } catch (const std::exception& e) {
            Logger::error("Invalid port number: " + std::string(argv[2]));
            config.root_path = ""; // Mark as invalid
            return config;
        }
    }

    // Optional: debug flag
    if (argc >= 4) {
        std::string debug_flag = argv[3];
        std::transform(debug_flag.begin(), debug_flag.end(), debug_flag.begin(), ::tolower);
        if (debug_flag == "--debug" || debug_flag == "-d") {
            config.enable_debug_logging = true;
        }
    }

    // Validate the path exists
    if (!isValidPath(config.root_path)) {
        Logger::error("Invalid or non-existent root path: " + config.root_path);
        config.root_path = ""; // Mark as invalid
        return config;
    }

    return config;
}

bool ConfigManager::isValidPath(const std::string& path) {
    return !path.empty() &&
           path != "." &&
           path != ".." &&
           FileUtils::exists(path) &&
           FileUtils::isDirectory(path);
}

bool ConfigManager::isValidPort(int port) {
    return port > 0 && port <= 65535;
}

void ConfigManager::printUsage(const char* program_name) {
    std::cout << "UTEC Conference Server - Local Video Streaming\n";
    std::cout << "Usage: " << program_name << " <recordings_path> [port] [--debug]\n";
    std::cout << "\n";
    std::cout << "Arguments:\n";
    std::cout << "  recordings_path  Path to your video recordings directory\n";
    std::cout << "  port            Server port (default: 8080, range: 1-65535)\n";
    std::cout << "  --debug         Enable debug logging\n";
    std::cout << "\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " /mnt/sharedFiles/UTEC/recordings\n";
    std::cout << "  " << program_name << " /mnt/sharedFiles/UTEC/recordings 9000\n";
    std::cout << "  " << program_name << " /mnt/sharedFiles/UTEC/recordings 8080 --debug\n";
    std::cout << "\n";
    std::cout << "Expected directory structure:\n";
    std::cout << "  recordings/\n";
    std::cout << "  └── YYYY/\n";
    std::cout << "      └── Semester_N/\n";
    std::cout << "          └── Course_Name/\n";
    std::cout << "              ├── Week_XX_TYPE.mp4\n";
    std::cout << "              └── ...\n";
    std::cout << "\n";
    std::cout << "Supported video formats:\n";
    std::cout << "  MP4, AVI, MKV, MOV, WMV, FLV, WebM, M4V, 3GP, MPG, MPEG\n";
}

std::string ConfigManager::normalizeRootPath(const std::string& path) {
    if (path.empty()) return path;

    std::string normalized = FileUtils::getAbsolutePath(path);

    // Remove trailing slashes
    while (normalized.length() > 1 &&
           (normalized.back() == '/' || normalized.back() == '\\')) {
        normalized.pop_back();
    }

    return normalized;
}

} // namespace utec