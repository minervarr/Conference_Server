// src/server/http_server.h
#pragma once
#include "config/server_config.h"
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
        explicit HttpServer(const ServerConfig& config);
        ~HttpServer();

        bool start();
        void stop();
        bool isRunning() const;

        int getPort() const { return config_.port; }
        const ServerConfig& getConfig() const { return config_; }

    private:
        ServerConfig config_;
        bool running_;

        std::shared_ptr<DirectoryScanner> scanner_;
        std::shared_ptr<VideoApi> api_;
        std::shared_ptr<RouteHandler> routes_;

        std::unique_ptr<httplib::Server> server_;
        std::thread server_thread_;

        void setupRoutes();
        void printStartupInfo();
        bool validateConfiguration();
    };

} // namespace utec