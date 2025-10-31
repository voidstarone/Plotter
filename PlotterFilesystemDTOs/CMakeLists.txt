cmake_minimum_required(VERSION 3.20)
project(PlotterFilesystemDTOs VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the library
add_library(PlotterFilesystemDTOs
    src/FilesystemDTOUtils.cpp
)

# Include directories
target_include_directories(PlotterFilesystemDTOs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link against PlotterDTOs to inherit configuration
target_include_directories(PlotterFilesystemDTOs
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../PlotterDTOs/include
)

# Inherit compile definitions from PlotterDTOs
target_compile_definitions(PlotterFilesystemDTOs PUBLIC PLOTTER_ENABLE_FILESYSTEM)

# Installation rules
install(TARGETS PlotterFilesystemDTOs
    EXPORT PlotterFilesystemDTOsTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
)

# Enable testing
enable_testing()
add_subdirectory(tests)
