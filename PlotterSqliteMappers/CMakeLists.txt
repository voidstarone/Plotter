cmake_minimum_required(VERSION 3.16)
project(PlotterSqliteMappers VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
set(PLOTTER_REPOSITORIES_DIR "${CMAKE_SOURCE_DIR}/../PlotterRepositories")
set(PLOTTER_ENTITIES_DIR "${CMAKE_SOURCE_DIR}/../PlotterEntities")
set(PLOTTER_DTOS_DIR "${CMAKE_SOURCE_DIR}/../PlotterDTOs")
set(PLOTTER_SQLITE_DTOS_DIR "${CMAKE_SOURCE_DIR}/../PlotterSqliteDTOs")

# Check if required directories exist
if(NOT EXISTS "${PLOTTER_REPOSITORIES_DIR}")
    message(FATAL_ERROR "PlotterRepositories directory not found at ${PLOTTER_REPOSITORIES_DIR}")
endif()

if(NOT EXISTS "${PLOTTER_ENTITIES_DIR}")
    message(FATAL_ERROR "PlotterEntities directory not found at ${PLOTTER_ENTITIES_DIR}")
endif()

if(NOT EXISTS "${PLOTTER_DTOS_DIR}")
    message(FATAL_ERROR "PlotterDTOs directory not found at ${PLOTTER_DTOS_DIR}")
endif()

if(NOT EXISTS "${PLOTTER_SQLITE_DTOS_DIR}")
    message(FATAL_ERROR "PlotterSqliteDTOs directory not found at ${PLOTTER_SQLITE_DTOS_DIR}")
endif()

# Create the mapper library
add_library(${PROJECT_NAME} STATIC
    src/SqliteProjectMapper.cpp
    src/SqliteFolderMapper.cpp
    src/SqliteNoteMapper.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${PLOTTER_REPOSITORIES_DIR}/include
    ${PLOTTER_ENTITIES_DIR}/include
    ${PLOTTER_DTOS_DIR}/include
    ${PLOTTER_SQLITE_DTOS_DIR}/include
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
    EXPORT PlotterSqliteMappersTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
