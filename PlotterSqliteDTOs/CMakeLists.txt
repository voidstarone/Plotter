cmake_minimum_required(VERSION 3.20)
project(PlotterSqliteDTOs VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the library
add_library(PlotterSqliteDTOs
    src/SqliteDTOUtils.cpp
)

# Include directories
target_include_directories(PlotterSqliteDTOs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link against PlotterDTOs to inherit configuration
target_include_directories(PlotterSqliteDTOs
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../PlotterDTOs/include
)

# Inherit compile definitions from PlotterDTOs
target_compile_definitions(PlotterSqliteDTOs PUBLIC PLOTTER_ENABLE_SQLITE)

# Installation rules
install(TARGETS PlotterSqliteDTOs
    EXPORT PlotterSqliteDTOsTargets
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
