cmake_minimum_required(VERSION 3.20)
project(PlotterDTOs VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build options for data source selection
option(PLOTTER_ENABLE_SQLITE "Enable SQLite data source support" ON)
option(PLOTTER_ENABLE_FILESYSTEM "Enable Filesystem data source support" ON)

# Ensure at least one data source is enabled
if(NOT PLOTTER_ENABLE_SQLITE AND NOT PLOTTER_ENABLE_FILESYSTEM)
    message(FATAL_ERROR "At least one data source must be enabled")
endif()

# Display configuration
message(STATUS "Plotter Configuration:")
message(STATUS "  SQLite data source: ${PLOTTER_ENABLE_SQLITE}")
message(STATUS "  Filesystem data source: ${PLOTTER_ENABLE_FILESYSTEM}")

# Create interface library for DTOs (header-only)
add_library(PlotterDTOs INTERFACE)

target_include_directories(PlotterDTOs
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Define macros based on options
if(PLOTTER_ENABLE_SQLITE)
    target_compile_definitions(PlotterDTOs INTERFACE PLOTTER_ENABLE_SQLITE)
endif()

if(PLOTTER_ENABLE_FILESYSTEM)
    target_compile_definitions(PlotterDTOs INTERFACE PLOTTER_ENABLE_FILESYSTEM)
endif()

# Installation rules
install(TARGETS PlotterDTOs
    EXPORT PlotterDTOsTargets
)

install(DIRECTORY include/
    DESTINATION include
)

install(EXPORT PlotterDTOsTargets
    FILE PlotterDTOsTargets.cmake
    NAMESPACE Plotter::
    DESTINATION lib/cmake/PlotterDTOs
)
