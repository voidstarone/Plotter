cmake_minimum_required(VERSION 3.10)
project(PlotterLogger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Accept configuration options (for compatibility with build system)
# These don't affect Logger, but prevent CMake warnings
set(PLOTTER_ENABLE_SQLITE ON CACHE BOOL "Enable SQLite data source support")
set(PLOTTER_ENABLE_FILESYSTEM ON CACHE BOOL "Enable Filesystem data source support")

# Create library with implementation
add_library(PlotterLogger STATIC
    src/ConsoleLogger.cpp
)

target_include_directories(PlotterLogger PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Install headers
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
