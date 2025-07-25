// src/config/server_config.h
#pragma once
#include <string>
#include <vector>
#include <cstddef>

namespace utec {

    struct ServerConfig {
        // Server settings
        std::string root_path;
        int port = 8080;
        std::string bind_address = "0.0.0.0";

        // Performance settings
        int read_timeout = 300;  // 5 minutes
        int write_timeout = 300; // 5 minutes
        size_t max_file_size = 5ULL * 1024 * 1024 * 1024; // 5GB

        // Security settings
        std::vector<std::string> allowed_extensions = {
            "mp4", "avi", "mkv", "mov", "wmv", "flv", "webm", "m4v", "3gp", "mpg", "mpeg"
        };
        bool enable_range_requests = true;
        bool enable_cors = true;

        // Logging settings
        bool enable_logging = true;
        bool enable_debug_logging = false;

        // Cache settings
        bool enable_library_cache = true;
        int cache_refresh_interval = 300; // 5 minutes

        // Validation
        bool isValid() const;
        std::string getValidationError() const;
    };

    class ConfigManager {
    public:
        static ServerConfig createDefault();
        static ServerConfig loadFromArgs(int argc, char* argv[]);
        static bool isValidPath(const std::string& path);
        static bool isValidPort(int port);

    private:
        static void printUsage(const char* program_name);
        static std::string normalizeRootPath(const std::string& path);
    };

} // namespace utec