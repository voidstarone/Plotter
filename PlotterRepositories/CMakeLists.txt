cmake_minimum_required(VERSION 3.16)
project(PlotterRepositories VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the dependencies
set(PLOTTER_USECASES_DIR "${CMAKE_SOURCE_DIR}/../PlotterUseCases")
set(PLOTTER_ENTITIES_DIR "${CMAKE_SOURCE_DIR}/../PlotterEntities")
set(PLOTTER_DTOS_DIR "${CMAKE_SOURCE_DIR}/../PlotterDTOs")

# Check if required directories exist
if(NOT EXISTS "${PLOTTER_USECASES_DIR}")
    message(FATAL_ERROR "PlotterUseCases directory not found at ${PLOTTER_USECASES_DIR}")
endif()

if(NOT EXISTS "${PLOTTER_ENTITIES_DIR}")
    message(FATAL_ERROR "PlotterEntities directory not found at ${PLOTTER_ENTITIES_DIR}")
endif()

if(NOT EXISTS "${PLOTTER_DTOS_DIR}")
    message(FATAL_ERROR "PlotterDTOs directory not found at ${PLOTTER_DTOS_DIR}")
endif()

# Create INTERFACE library (header-only since templates are in headers)
add_library(${PROJECT_NAME} INTERFACE)

# Include directories
target_include_directories(${PROJECT_NAME} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PLOTTER_USECASES_DIR}/include>
    $<BUILD_INTERFACE:${PLOTTER_ENTITIES_DIR}/include>
    $<BUILD_INTERFACE:${PLOTTER_DTOS_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Install headers
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Install interface target
install(TARGETS ${PROJECT_NAME}
    EXPORT PlotterRepositoriesTargets
)

# Examples showing how to use these interfaces
option(BUILD_EXAMPLES "Build example applications" ON)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Tests
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
