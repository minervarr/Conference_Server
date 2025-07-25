// UTEC Conference Server - Frontend JavaScript

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
        { text: `${semester.name} - ${year}`, action: `showSemester('${year}', semester)` }
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
        { text: `${semester} - ${year}`, action: `showSemester('${year}', {name: '${semester}'})` },
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
                <button onclick="showCourse('${year}', '${semester}', course)" class="btn btn-primary">Try Again</button>
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
});