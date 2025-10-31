cmake_minimum_required(VERSION 3.10)
project(PlotterLogger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This is a header-only library
add_library(PlotterLogger INTERFACE)

target_include_directories(PlotterLogger INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Install headers
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)
