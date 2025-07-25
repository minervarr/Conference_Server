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

// ================================================================

// src/web/template_engine.cpp
#include "web/template_engine.h"
#include "utils/string_utils.h"

namespace utec {

std::string TemplateEngine::render(const std::string& template_str,
                                  const std::map<std::string, std::string>& variables) {
    std::string result = template_str;

    for (const auto& pair : variables) {
        replaceVariable(result, pair.first, pair.second);
    }

    return result;
}

void TemplateEngine::replaceVariable(std::string& content,
                                    const std::string& var_name,
                                    const std::string& value) {
    std::string placeholder = "{{" + var_name + "}}";
    content = StringUtils::replaceAll(content, placeholder, value);
}
    </script>
</body>
</html>)";

const std::string EmbeddedResources::MAIN_CSS = R"(/* UTEC Conference Server - Parrot Green Theme */
:root {
    /* Parrot Green Color Palette */
    --primary-green: #00C851;
    --dark-green: #00A043;
    --light-green: #69F0AE;
    --pale-green: #E8F5E8;
    --accent-green: #4CAF50;

    /* Neutral Colors */
    --white: #FFFFFF;
    --light-gray: #F5F5F5;
    --gray: #9E9E9E;
    --dark-gray: #424242;
    --black: #212121;

    /* Spacing */
    --spacing-xs: 0.5rem;
    --spacing-sm: 1rem;
    --spacing-md: 1.5rem;
    --spacing-lg: 2rem;
    --spacing-xl: 3rem;

    /* Border Radius */
    --radius-sm: 4px;
    --radius-md: 8px;
    --radius-lg: 12px;
    --radius-xl: 20px;

    /* Shadows */
    --shadow-sm: 0 2px 4px rgba(0, 0, 0, 0.1);
    --shadow-md: 0 4px 12px rgba(0, 0, 0, 0.15);
    --shadow-lg: 0 8px 24px rgba(0, 0, 0, 0.2);
}

/* Reset and Base Styles */
* {
    margin: 0;
    padding: 0;
    box-sizing: border-box;
}

body {
    font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, 'Roboto', sans-serif;
    line-height: 1.6;
    color: var(--black);
    background: linear-gradient(135deg, var(--pale-green) 0%, var(--white) 100%);
    min-height: 100vh;
}

.container {
    max-width: 1200px;
    margin: 0 auto;
    padding: 0 var(--spacing-md);
}

/* Header */
.header {
    background: linear-gradient(135deg, var(--primary-green) 0%, var(--dark-green) 100%);
    color: var(--white);
    padding: var(--spacing-lg) 0;
    box-shadow: var(--shadow-md);
}

.logo {
    font-size: 2.5rem;
    font-weight: 700;
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
    margin-bottom: var(--spacing-xs);
}

.logo-icon {
    font-size: 3rem;
    filter: drop-shadow(2px 2px 4px rgba(0, 0, 0, 0.3));
}

.header-subtitle {
    font-size: 1.1rem;
    opacity: 0.9;
    font-weight: 300;
}

/* Main Content */
.main {
    flex: 1;
    padding: var(--spacing-xl) 0;
}

/* Loading State */
.loading {
    text-align: center;
    padding: var(--spacing-xl);
}

.loading-spinner {
    width: 50px;
    height: 50px;
    border: 4px solid var(--light-green);
    border-top: 4px solid var(--primary-green);
    border-radius: 50%;
    animation: spin 1s linear infinite;
    margin: 0 auto var(--spacing-md);
}

@keyframes spin {
    0% { transform: rotate(0deg); }
    100% { transform: rotate(360deg); }
}

/* Error State */
.error-state {
    text-align: center;
    padding: var(--spacing-xl);
    background: var(--white);
    border-radius: var(--radius-lg);
    box-shadow: var(--shadow-sm);
    max-width: 500px;
    margin: 0 auto;
}

.error-icon {
    font-size: 4rem;
    margin-bottom: var(--spacing-md);
}

/* Search */
.search-container {
    margin-bottom: var(--spacing-lg);
}

.search-box {
    display: flex;
    max-width: 500px;
    margin: 0 auto;
    background: var(--white);
    border-radius: var(--radius-xl);
    overflow: hidden;
    box-shadow: var(--shadow-md);
}

.search-input {
    flex: 1;
    padding: var(--spacing-md);
    border: none;
    font-size: 1rem;
    outline: none;
}

.search-btn {
    background: var(--primary-green);
    color: var(--white);
    border: none;
    padding: var(--spacing-md) var(--spacing-lg);
    cursor: pointer;
    font-size: 1.2rem;
    transition: background-color 0.3s ease;
}

.search-btn:hover {
    background: var(--dark-green);
}

/* Breadcrumb */
.breadcrumb {
    margin-bottom: var(--spacing-lg);
    font-size: 0.9rem;
}

.breadcrumb-item {
    color: var(--gray);
    cursor: pointer;
    transition: color 0.3s ease;
}

.breadcrumb-item:hover {
    color: var(--primary-green);
}

.breadcrumb-item.active {
    color: var(--primary-green);
    font-weight: 600;
}

.breadcrumb-item:not(:last-child)::after {
    content: ' › ';
    margin: 0 var(--spacing-xs);
    color: var(--gray);
}

/* Card Grid */
.cards-grid {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));
    gap: var(--spacing-lg);
}

.card {
    background: var(--white);
    border-radius: var(--radius-lg);
    padding: var(--spacing-lg);
    box-shadow: var(--shadow-sm);
    transition: all 0.3s ease;
    cursor: pointer;
    border: 2px solid transparent;
}

.card:hover {
    transform: translateY(-4px);
    box-shadow: var(--shadow-lg);
    border-color: var(--light-green);
}

.card-title {
    font-size: 1.3rem;
    font-weight: 600;
    color: var(--primary-green);
    margin-bottom: var(--spacing-sm);
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
}

.card-icon {
    font-size: 1.5rem;
}

.card-meta {
    color: var(--gray);
    font-size: 0.9rem;
    margin-bottom: var(--spacing-sm);
}

.card-description {
    color: var(--dark-gray);
    line-height: 1.5;
}

/* Videos Grid */
.videos-grid {
    display: grid;
    grid-template-columns: repeat(auto-fill, minmax(350px, 1fr));
    gap: var(--spacing-md);
}

.video-card {
    background: var(--white);
    border-radius: var(--radius-md);
    padding: var(--spacing-md);
    box-shadow: var(--shadow-sm);
    transition: all 0.3s ease;
    cursor: pointer;
    border-left: 4px solid var(--primary-green);
}

.video-card:hover {
    transform: translateY(-2px);
    box-shadow: var(--shadow-md);
    border-left-color: var(--dark-green);
}

.video-title {
    font-weight: 600;
    margin-bottom: var(--spacing-xs);
    color: var(--black);
}

.video-meta {
    font-size: 0.8rem;
    color: var(--gray);
    display: flex;
    justify-content: space-between;
    align-items: center;
}

.video-type {
    background: var(--light-green);
    color: var(--dark-green);
    padding: 2px 8px;
    border-radius: var(--radius-sm);
    font-weight: 500;
}

/* Buttons */
.btn {
    padding: var(--spacing-sm) var(--spacing-lg);
    border: none;
    border-radius: var(--radius-md);
    font-size: 1rem;
    font-weight: 500;
    cursor: pointer;
    transition: all 0.3s ease;
    text-decoration: none;
    display: inline-block;
}

.btn-primary {
    background: var(--primary-green);
    color: var(--white);
}

.btn-primary:hover {
    background: var(--dark-green);
    transform: translateY(-2px);
}

.btn-secondary {
    background: var(--light-gray);
    color: var(--dark-gray);
}

.btn-secondary:hover {
    background: var(--gray);
    color: var(--white);
}

/* Modal */
.modal {
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: rgba(0, 0, 0, 0.8);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 1000;
}

.modal-content {
    background: var(--white);
    border-radius: var(--radius-lg);
    max-width: 90vw;
    max-height: 90vh;
    overflow: hidden;
    box-shadow: var(--shadow-lg);
}

.modal-header {
    padding: var(--spacing-md) var(--spacing-lg);
    background: var(--primary-green);
    color: var(--white);
    display: flex;
    justify-content: space-between;
    align-items: center;
}

.modal-body {
    padding: var(--spacing-lg);
}

.close-btn {
    background: none;
    border: none;
    color: var(--white);
    font-size: 2rem;
    cursor: pointer;
    padding: 0;
    width: 40px;
    height: 40px;
    display: flex;
    align-items: center;
    justify-content: center;
    border-radius: 50%;
    transition: background-color 0.3s ease;
}

.close-btn:hover {
    background: rgba(255, 255, 255, 0.2);
}

.video-actions {
    margin-top: var(--spacing-md);
    display: flex;
    gap: var(--spacing-sm);
    justify-content: center;
}

/* Footer */
.footer {
    background: var(--dark-gray);
    color: var(--white);
    text-align: center;
    padding: var(--spacing-lg) 0;
    margin-top: var(--spacing-xl);
}

/* Responsive Design */
@media (max-width: 768px) {
    .container {
        padding: 0 var(--spacing-sm);
    }

    .logo {
        font-size: 2rem;
    }

    .cards-grid {
        grid-template-columns: 1fr;
    }

    .videos-grid {
        grid-template-columns: 1fr;
    }

    .modal-content {
        max-width: 95vw;
    }

    .video-actions {
        flex-direction: column;
    }
}

/* Animations */
@keyframes fadeIn {
    from { opacity: 0; transform: translateY(20px); }
    to { opacity: 1; transform: translateY(0); }
}

.fade-in {
    animation: fadeIn 0.5s ease-out;
}

/* Utility Classes */
.text-center { text-align: center; }
.mb-lg { margin-bottom: var(--spacing-lg); }
.mt-lg { margin-top: var(--spacing-lg); })";

const std::string EmbeddedResources::MAIN_JS = R"(// UTEC Conference Server - Frontend JavaScript

let currentLibrary = null;
let currentView = 'library';
let currentVideo = null;

// Utility Functions
function formatFileSize(bytes) {
    if (bytes === 0) return '0 Bytes';
    const k = 1024;
    const sizes = ['Bytes', 'KB', 'MB', 'GB'];
    const i = Math.floor(Math.log(bytes) / Math.log(k));
    return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
}

function extractVideoType(filename) {
    if (filename.includes('TEORÍA')) return 'Theory';
    if (filename.includes('LABORATORIO')) return 'Lab';
    if (filename.includes('VIRTUAL')) return 'Virtual';
    return 'Video';
}

function extractWeekNumber(filename) {
    const match = filename.match(/Week_(\d+)/);
    return match ? `Week ${match[1]}` : '';
}

// API Functions
async function fetchAPI(endpoint) {
    try {
        const response = await fetch(`${window.SERVER_URL}/api/${endpoint}`);
        if (!response.ok) {
            throw new Error(`HTTP ${response.status}: ${response.statusText}`);
        }
        return await response.json();
    } catch (error) {
        console.error('API Error:', error);
        throw error;
    }
}

// Load video library
async function loadLibrary() {
    showElement('loading');
    hideElement('error');
    hideElement('library-view');
    hideElement('search-container');
    hideElement('breadcrumb');

    try {
        const response = await fetchAPI('library');
        currentLibrary = response.data;
        showLibrary();
    } catch (error) {
        showError('Failed to load video library: ' + error.message);
    }
}

// Display library view
function showLibrary() {
    currentView = 'library';
    updateBreadcrumb([{ text: 'Library', action: 'showLibrary()' }]);

    hideElement('loading');
    hideElement('error');
    hideElement('course-view');
    showElement('library-view');
    showElement('search-container');
    showElement('breadcrumb');

    const container = document.getElementById('years-container');
    container.innerHTML = '';

    if (!currentLibrary || !currentLibrary.years.length) {
        container.innerHTML = `
            <div class="text-center">
                <h2>No videos found</h2>
                <p>Make sure your videos are organized in the correct structure:</p>
                <p><code>recordings/YYYY/Semester_N/Course_Name/*.mp4</code></p>
            </div>
        `;
        return;
    }

    currentLibrary.years.forEach(year => {
        year.semesters.forEach(semester => {
            const semesterCard = createSemesterCard(year.year, semester);
            container.appendChild(semesterCard);
        });
    });

    // Add fade-in animation
    container.querySelectorAll('.card').forEach((card, index) => {
        card.style.animationDelay = `${index * 0.1}s`;
        card.classList.add('fade-in');
    });
}

// Create course card
function createCourseCard(year, semesterName, course) {
    const card = document.createElement('div');
    card.className = 'card';
    card.onclick = () => showCourse(year, semesterName, course);

    card.innerHTML = `
        <div class="card-title">
            <span class="card-icon">🎥</span>
            ${course.name}
        </div>
        <div class="card-meta">
            ${course.video_count} videos available
        </div>
        <div class="card-description">
            Click to view all videos for this course
        </div>
    `;

    return card;
}

// Show course videos
async function showCourse(year, semester, course) {
    currentView = 'course';
    updateBreadcrumb([
        { text: 'Library', action: 'showLibrary()' },
        { text: `${semester} - ${year}`, action: `showSemester('${year}', ${JSON.stringify({name: semester}).replace(/'/g, "\\'")})` },
        { text: course.name, action: '' }
    ]);

    hideElement('library-view');
    showElement('course-view');

    const container = document.getElementById('videos-container');
    container.innerHTML = '<div class="loading"><div class="loading-spinner"></div><p>Loading videos...</p></div>';

    try {
        const response = await fetchAPI(`video?year=${encodeURIComponent(year)}&semester=${encodeURIComponent(semester)}&course=${encodeURIComponent(course.name)}&video=`);

        container.innerHTML = '';

        if (!response.data.videos || response.data.videos.length === 0) {
            container.innerHTML = `
                <div class="text-center">
                    <h3>No videos found in this course</h3>
                    <p>Videos may have been moved or are in an unsupported format.</p>
                </div>
            `;
            return;
        }

        response.data.videos.forEach(video => {
            const videoCard = createVideoCard(video);
            container.appendChild(videoCard);
        });

        // Add fade-in animation
        container.querySelectorAll('.video-card').forEach((card, index) => {
            card.style.animationDelay = `${index * 0.05}s`;
            card.classList.add('fade-in');
        });

    } catch (error) {
        container.innerHTML = `
            <div class="error-state">
                <div class="error-icon">⚠️</div>
                <h3>Failed to load videos</h3>
                <p>${error.message}</p>
                <button onclick="showCourse('${year}', '${semester}', ${JSON.stringify(course).replace(/'/g, "\\'")}))" class="btn btn-primary">Try Again</button>
            </div>
        `;
    }
}

// Create video card
function createVideoCard(video) {
    const card = document.createElement('div');
    card.className = 'video-card';
    card.onclick = () => playVideo(video);

    const weekNumber = extractWeekNumber(video.name);
    const videoType = extractVideoType(video.name);
    const fileSize = formatFileSize(video.size);

    card.innerHTML = `
        <div class="video-title">${video.name}</div>
        <div class="video-meta">
            <span>${weekNumber}</span>
            <span class="video-type">${videoType}</span>
            <span>${fileSize}</span>
        </div>
    `;

    return card;
}

// Play video
function playVideo(video) {
    currentVideo = video;
    const modal = document.getElementById('video-modal');
    const player = document.getElementById('video-player');
    const title = document.getElementById('video-title');

    title.textContent = video.name;
    player.src = `${window.SERVER_URL}/stream/${encodeURIComponent(video.path)}`;

    showElement('video-modal');

    // Focus on video player
    setTimeout(() => player.focus(), 100);
}

// Close video modal
function closeVideoModal() {
    hideElement('video-modal');
    const player = document.getElementById('video-player');
    player.pause();
    player.src = '';
    currentVideo = null;
}

// Open video in external app
function openInApp() {
    if (!currentVideo) return;

    const videoUrl = `${window.SERVER_URL}/stream/${encodeURIComponent(currentVideo.path)}`;

    // Create a temporary link to trigger app opening
    const link = document.createElement('a');
    link.href = videoUrl;
    link.target = '_blank';
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);

    // Show helpful message
    showToast('Video URL opened! Your device should prompt to open in a video app.');
}

// Copy video URL
function copyVideoUrl() {
    if (!currentVideo) return;

    const videoUrl = `${window.SERVER_URL}/stream/${encodeURIComponent(currentVideo.path)}`;

    navigator.clipboard.writeText(videoUrl).then(() => {
        showToast('Video URL copied to clipboard!');
    }).catch(() => {
        // Fallback for older browsers
        const textArea = document.createElement('textarea');
        textArea.value = videoUrl;
        document.body.appendChild(textArea);
        textArea.select();
        document.execCommand('copy');
        document.body.removeChild(textArea);
        showToast('Video URL copied to clipboard!');
    });
}

// Search videos
async function searchVideos() {
    const query = document.getElementById('search-input').value.trim();
    if (!query) return;

    // Simple client-side search for now
    if (!currentLibrary) return;

    const results = [];
    currentLibrary.years.forEach(year => {
        year.semesters.forEach(semester => {
            semester.courses.forEach(course => {
                // Note: We don't have video details in the library response
                // This would need to be implemented server-side for full functionality
                if (course.name.toLowerCase().includes(query.toLowerCase())) {
                    results.push({
                        type: 'course',
                        year: year.year,
                        semester: semester.name,
                        course: course
                    });
                }
            });
        });
    });

    showSearchResults(query, results);
}

// Show search results
function showSearchResults(query, results) {
    currentView = 'search';
    updateBreadcrumb([
        { text: 'Library', action: 'showLibrary()' },
        { text: `Search: "${query}"`, action: '' }
    ]);

    hideElement('library-view');
    showElement('course-view');

    const container = document.getElementById('videos-container');
    container.innerHTML = '';

    if (results.length === 0) {
        container.innerHTML = `
            <div class="text-center">
                <h3>No results found</h3>
                <p>Try a different search term or browse the library.</p>
            </div>
        `;
        return;
    }

    results.forEach(result => {
        if (result.type === 'course') {
            const courseCard = createCourseCard(result.year, result.semester, result.course);
            container.appendChild(courseCard);
        }
    });
}

// Update breadcrumb navigation
function updateBreadcrumb(items) {
    const breadcrumb = document.getElementById('breadcrumb');
    breadcrumb.innerHTML = '';

    items.forEach((item, index) => {
        const span = document.createElement('span');
        span.className = 'breadcrumb-item';
        span.textContent = item.text;

        if (item.action && index < items.length - 1) {
            span.onclick = () => eval(item.action);
        } else {
            span.classList.add('active');
        }

        breadcrumb.appendChild(span);
    });
}

// Show toast notification
function showToast(message) {
    // Create toast element
    const toast = document.createElement('div');
    toast.style.cssText = `
        position: fixed;
        top: 20px;
        right: 20px;
        background: var(--primary-green);
        color: white;
        padding: 1rem 1.5rem;
        border-radius: 8px;
        box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
        z-index: 1001;
        font-weight: 500;
        transform: translateX(100%);
        transition: transform 0.3s ease;
    `;
    toast.textContent = message;

    document.body.appendChild(toast);

    // Animate in
    setTimeout(() => {
        toast.style.transform = 'translateX(0)';
    }, 10);

    // Remove after 3 seconds
    setTimeout(() => {
        toast.style.transform = 'translateX(100%)';
        setTimeout(() => {
            document.body.removeChild(toast);
        }, 300);
    }, 3000);
}

// Utility functions for showing/hiding elements
function showElement(id) {
    document.getElementById(id).style.display = 'block';
}

function hideElement(id) {
    document.getElementById(id).style.display = 'none';
}

function showError(message) {
    document.getElementById('error-message').textContent = message;
    hideElement('loading');
    hideElement('library-view');
    hideElement('course-view');
    showElement('error');
}

// Keyboard shortcuts
document.addEventListener('keydown', function(e) {
    // Escape key closes modal
    if (e.key === 'Escape' && currentVideo) {
        closeVideoModal();
    }

    // Enter key triggers search
    if (e.key === 'Enter' && document.activeElement.id === 'search-input') {
        searchVideos();
    }
});

// Handle search input
document.addEventListener('DOMContentLoaded', function() {
    const searchInput = document.getElementById('search-input');
    if (searchInput) {
        searchInput.addEventListener('keyup', function(e) {
            if (e.key === 'Enter') {
                searchVideos();
            }
        });
    }
});ationDelay = `${index * 0.1}s`;
        card.classList.add('fade-in');
    });
}

// Create semester card
function createSemesterCard(year, semester) {
    const totalVideos = semester.courses.reduce((sum, course) => sum + course.video_count, 0);

    const card = document.createElement('div');
    card.className = 'card';
    card.onclick = () => showSemester(year, semester);

    card.innerHTML = `
        <div class="card-title">
            <span class="card-icon">📚</span>
            ${semester.name} - ${year}
        </div>
        <div class="card-meta">
            ${semester.courses.length} courses • ${totalVideos} videos
        </div>
        <div class="card-description">
            ${semester.courses.map(course => course.name).join(' • ')}
        </div>
    `;

    return card;
}

// Show semester courses
function showSemester(year, semester) {
    currentView = 'semester';
    updateBreadcrumb([
        { text: 'Library', action: 'showLibrary()' },
        { text: `${semester.name} - ${year}`, action: `showSemester('${year}', ${JSON.stringify(semester).replace(/'/g, "\\'")})` }
    ]);

    hideElement('library-view');
    showElement('course-view');

    const container = document.getElementById('videos-container');
    container.innerHTML = '';

    semester.courses.forEach(course => {
        const courseCard = createCourseCard(year, semester.name, course);
        container.appendChild(courseCard);
    });

    // Add fade-in animation
    container.querySelectorAll('.card').forEach((card, index) => {
        card.style.anim