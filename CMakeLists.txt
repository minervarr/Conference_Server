# CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(UTEC_Conference_Server VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build configuration
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Include directories
include_directories(src)
include_directories(libs)  # For httplib.h

# Source files organized by modules
set(MAIN_SOURCES
        src/main.cpp
)

set(CONFIG_SOURCES
        src/config/server_config.cpp
)

set(CORE_SOURCES
        src/core/error_handler.cpp
)

set(SECURITY_SOURCES
        src/security/path_validator.cpp
)

set(SERVER_SOURCES
        src/server/http_server.cpp
        src/server/route_handler.cpp
)

set(FILESYSTEM_SOURCES
        src/filesystem/directory_scanner.cpp
        src/filesystem/file_utils.cpp
)

set(API_SOURCES
        src/api/video_api.cpp
        src/api/json_response.cpp
)

set(WEB_SOURCES
        src/web/embedded_resources.cpp
        src/web/template_engine.cpp
)

set(UTILS_SOURCES
        src/utils/string_utils.cpp
        src/utils/logger.cpp
)

# Headers (for IDE support)
set(CONFIG_HEADERS
        src/config/server_config.h
)

set(CORE_HEADERS
        src/core/error_handler.h
)

set(SECURITY_HEADERS
        src/security/path_validator.h
)

set(SERVER_HEADERS
        src/server/http_server.h
        src/server/route_handler.h
)

set(FILESYSTEM_HEADERS
        src/filesystem/directory_scanner.h
        src/filesystem/file_utils.h
)

set(API_HEADERS
        src/api/video_api.h
        src/api/json_response.h
)

set(WEB_HEADERS
        src/web/embedded_resources.h
        src/web/template_engine.h
)

set(UTILS_HEADERS
        src/utils/string_utils.h
        src/utils/logger.h
        src/utils/types.h
)

# Combine all sources
set(ALL_SOURCES
        ${MAIN_SOURCES}
        ${CONFIG_SOURCES}
        ${CORE_SOURCES}
        ${SECURITY_SOURCES}
        ${SERVER_SOURCES}
        ${FILESYSTEM_SOURCES}
        ${API_SOURCES}
        ${WEB_SOURCES}
        ${UTILS_SOURCES}
)

set(ALL_HEADERS
        ${CONFIG_HEADERS}
        ${CORE_HEADERS}
        ${SECURITY_HEADERS}
        ${SERVER_HEADERS}
        ${FILESYSTEM_HEADERS}
        ${API_HEADERS}
        ${WEB_HEADERS}
        ${UTILS_HEADERS}
)

# Create object libraries for better modularity (optional)
add_library(config_module OBJECT ${CONFIG_SOURCES})
add_library(core_module OBJECT ${CORE_SOURCES})
add_library(security_module OBJECT ${SECURITY_SOURCES})
add_library(server_module OBJECT ${SERVER_SOURCES})
add_library(filesystem_module OBJECT ${FILESYSTEM_SOURCES})
add_library(api_module OBJECT ${API_SOURCES})
add_library(web_module OBJECT ${WEB_SOURCES})
add_library(utils_module OBJECT ${UTILS_SOURCES})

# Main executable
add_executable(${PROJECT_NAME}
        ${MAIN_SOURCES}
        $<TARGET_OBJECTS:config_module>
        $<TARGET_OBJECTS:core_module>
        $<TARGET_OBJECTS:security_module>
        $<TARGET_OBJECTS:server_module>
        $<TARGET_OBJECTS:filesystem_module>
        $<TARGET_OBJECTS:api_module>
        $<TARGET_OBJECTS:web_module>
        $<TARGET_OBJECTS:utils_module>
)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
            -Wall -Wextra -Wpedantic
            $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
            $<$<CONFIG:Release>:-O3 -DNDEBUG>
    )

    # Apply same flags to object libraries
    foreach(module config core security server filesystem api web utils)
        target_compile_options(${module}_module PRIVATE
                -Wall -Wextra -Wpedantic
                $<$<CONFIG:Debug>:-g -O0 -DDEBUG>
                $<$<CONFIG:Release>:-O3 -DNDEBUG>
        )
    endforeach()
endif()

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
            /W4
            $<$<CONFIG:Debug>:/Od /DEBUG>
            $<$<CONFIG:Release>:/O2 /DNDEBUG>
    )

    # Apply same flags to object libraries
    foreach(module config core security server filesystem api web utils)
        target_compile_options(${module}_module PRIVATE
                /W4
                $<$<CONFIG:Debug>:/Od /DEBUG>
                $<$<CONFIG:Release>:/O2 /DNDEBUG>
        )
    endforeach()
endif()

# Platform-specific linking
if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
else()
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

# Create necessary directories
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/config)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/core)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/security)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/server)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/filesystem)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/api)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/web)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/src/utils)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/libs)

# Install target (optional)
install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION bin
)

# Print build information
message(STATUS "UTEC Conference Server Configuration:")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")

# Generate compile_commands.json for better IDE support
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)