cmake_minimum_required(VERSION 3.10)
project(PlotterDTOs VERSION 1.0.0)

# Header-only library for base DTOs
add_library(PlotterDTOs INTERFACE)

target_include_directories(PlotterDTOs INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Set C++ standard
target_compile_features(PlotterDTOs INTERFACE cxx_std_17)
