// src/web/embedded_resources.cpp
#include "web/embedded_resources.h"
#include "web/template_engine.h"

namespace utec {

const std::string EmbeddedResources::INDEX_HTML = R"(<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>UTEC Conference Server</title>
    <link rel="stylesheet" href="/static/style.css">
    <link rel="icon" href="/favicon.ico" type="image/x-icon">
</head>
<body>
    <header class="header">
        <div class="container">
            <h1 class="logo">
                <span class="logo-icon">🎓</span>
                UTEC Conference Server
            </h1>
            <div class="header-subtitle">Your Personal Academic Video Library</div>
        </div>
    </header>

    <main class="main">
        <div class="container">
            <!-- Loading State -->
            <div id="loading" class="loading">
                <div class="loading-spinner"></div>
                <p>Loading your video library...</p>
            </div>

            <!-- Error State -->
            <div id="error" class="error-state" style="display: none;">
                <div class="error-icon">⚠️</div>
                <h2>Oops! Something went wrong</h2>
                <p id="error-message"></p>
                <button onclick="loadLibrary()" class="btn btn-primary">Try Again</button>
            </div>

            <!-- Search Bar -->
            <div id="search-container" class="search-container" style="display: none;">
                <div class="search-box">
                    <input type="text" id="search-input" placeholder="Search for videos..." class="search-input">
                    <button onclick="searchVideos()" class="search-btn">🔍</button>
                </div>
            </div>

            <!-- Breadcrumb Navigation -->
            <nav id="breadcrumb" class="breadcrumb" style="display: none;">
                <span class="breadcrumb-item active" onclick="showLibrary()">Library</span>
            </nav>

            <!-- Library View -->
            <div id="library-view" class="library-view" style="display: none;">
                <div id="years-container" class="cards-grid"></div>
            </div>

            <!-- Course View -->
            <div id="course-view" class="course-view" style="display: none;">
                <div id="videos-container" class="videos-grid"></div>
            </div>

            <!-- Video Player Modal -->
            <div id="video-modal" class="modal" style="display: none;">
                <div class="modal-content">
                    <div class="modal-header">
                        <h3 id="video-title"></h3>
                        <button onclick="closeVideoModal()" class="close-btn">×</button>
                    </div>
                    <div class="modal-body">
                        <video id="video-player" controls width="100%">
                            Your browser does not support the video tag.
                        </video>
                        <div class="video-actions">
                            <button onclick="openInApp()" class="btn btn-secondary">📱 Open in App</button>
                            <button onclick="copyVideoUrl()" class="btn btn-secondary">🔗 Copy Link</button>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </main>

    <footer class="footer">
        <div class="container">
            <p>&copy; 2025 UTEC Conference Server - Built with ❤️ for Learning</p>
        </div>
    </footer>

    <script src="/static/app.js"></script>
    <script>
        // Initialize the app
        document.addEventListener('DOMContentLoaded', function() {
            window.SERVER_URL = '{{SERVER_URL}}';
            loadLibrary();

std::string EmbeddedResources::getIndexHtml() {
    return INDEX_HTML;
}

std::string EmbeddedResources::getMainCss() {
    return MAIN_CSS;
}

std::string EmbeddedResources::getMainJs() {
    return MAIN_JS;
}

std::string EmbeddedResources::getFavicon() {
    // Simple base64 encoded favicon (16x16 green icon)
    return "data:image/x-icon;base64,AAABAAEAEBAQAAEABAAoAQAAFgAAACgAAAAQAAAAIAAAAAEABAAAAAAAgAAAAAAAAAAAAAAAEAAAAAAAAAD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8A////AP///wD///8AEREREREREREQEREREREQEREREREQEREREREQEREREREQEREREREQEREREREQEREREREQEREREREQEREREREREREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQEREQ==";
}

std::string EmbeddedResources::processTemplate(const std::string& html, const std::string& server_url) {
    return TemplateEngine::render(html, {
        {"SERVER_URL", server_url}
    });
}

} // namespace utec

