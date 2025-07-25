# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(UTEC_Conference_Server)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)
include_directories(libs)  # For httplib.h

# Source files organized by modules
set(SOURCES
        # Main entry point
        src/main.cpp

        # Server module
        src/server/http_server.cpp
        src/server/route_handler.cpp

        # Filesystem module
        src/filesystem/directory_scanner.cpp
        src/filesystem/file_utils.cpp

        # API module
        src/api/video_api.cpp
        src/api/json_response.cpp

        # Web module
        src/web/embedded_resources.cpp
        src/web/template_engine.cpp

        # Utils module
        src/utils/string_utils.cpp
        src/utils/logger.cpp
        src/utils/logger.h
        src/utils/string_utils.h
        src/filesystem/file_utils.h
        src/filesystem/directory_scanner.h
        src/api/video_api.h
        src/web/embedded_resources.h
        src/web/template_engine.h
        src/server/route_handler.h
        src/server/http_server.h
        src/api/video_api.cpp
        src/web/template_engine.cpp
        src/utils/string_utils.cpp
        src/filesystem/directory_scanner.cpp
)

# Headers (for IDE support, not required for compilation)
set(HEADERS
        # Server module
        src/server/http_server.h
        src/server/route_handler.h

        # Filesystem module
        src/filesystem/directory_scanner.h
        src/filesystem/file_utils.h

        # API module
        src/api/video_api.h
        src/api/json_response.h

        # Web module
        src/web/embedded_resources.h
        src/web/template_engine.h

        # Utils module
        src/utils/string_utils.h
        src/utils/logger.h
        src/utils/types.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

# Compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -O2)
endif()

# Platform-specific linking
if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
else()
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

# Create necessary directories if they don't exist
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/server)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/filesystem)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/api)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/web)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/utils)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/libs)