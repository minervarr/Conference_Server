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