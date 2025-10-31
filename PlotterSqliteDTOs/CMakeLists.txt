cmake_minimum_required(VERSION 3.16)
project(PlotterSqliteDTOs VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
set(PLOTTER_DTOS_DIR "${CMAKE_SOURCE_DIR}/../PlotterDTOs")

# Check if required directories exist
if(NOT EXISTS "${PLOTTER_DTOS_DIR}")
    message(FATAL_ERROR "PlotterDTOs directory not found at ${PLOTTER_DTOS_DIR}")
endif()

# Create the library
add_library(${PROJECT_NAME} STATIC
    src/SqliteDTOUtils.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${PLOTTER_DTOS_DIR}/include
)

# Tests
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install headers
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Install library
install(TARGETS ${PROJECT_NAME}
    EXPORT PlotterSqliteDTOsTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
