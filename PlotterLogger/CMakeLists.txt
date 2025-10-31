cmake_minimum_required(VERSION 3.10)
project(PlotterLogger)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
