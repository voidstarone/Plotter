cmake_minimum_required(VERSION 3.20)
project(PlotterSqliteMappers VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Accept configuration options (passed from build system)
set(PLOTTER_ENABLE_SQLITE ON CACHE BOOL "Enable SQLite data source support")
set(PLOTTER_ENABLE_FILESYSTEM ON CACHE BOOL "Enable Filesystem data source support")

# Add the library
add_library(PlotterSqliteMappers
    src/SqliteProjectMapper.cpp
    src/SqliteFolderMapper.cpp
    src/SqliteNoteMapper.cpp
)

# Include directories
target_include_directories(PlotterSqliteMappers
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Link dependencies
target_include_directories(PlotterSqliteMappers
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../PlotterDTOs/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../PlotterSqliteDTOs/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../PlotterEntities/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../PlotterRepositories/include
)

# Installation rules
install(TARGETS PlotterSqliteMappers
    EXPORT PlotterSqliteMappersTargets
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
