cmake_minimum_required(VERSION 3.16)
project(PlotterSqliteDataSource VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
set(PLOTTER_REPOSITORIES_DIR "${CMAKE_SOURCE_DIR}/../PlotterRepositories")
set(PLOTTER_DTOS_DIR "${CMAKE_SOURCE_DIR}/../PlotterDTOs")
set(PLOTTER_SQLITE_DTOS_DIR "${CMAKE_SOURCE_DIR}/../PlotterSqliteDTOs")

# Check if required directories exist
if(NOT EXISTS "${PLOTTER_REPOSITORIES_DIR}")
    message(FATAL_ERROR "PlotterRepositories directory not found at ${PLOTTER_REPOSITORIES_DIR}")
endif()

if(NOT EXISTS "${PLOTTER_DTOS_DIR}")
    message(FATAL_ERROR "PlotterDTOs directory not found at ${PLOTTER_DTOS_DIR}")
endif()

if(NOT EXISTS "${PLOTTER_SQLITE_DTOS_DIR}")
    message(FATAL_ERROR "PlotterSqliteDTOs directory not found at ${PLOTTER_SQLITE_DTOS_DIR}")
endif()

# Find SQLite3
find_package(SQLite3 REQUIRED)

# Create the library
add_library(${PROJECT_NAME} STATIC
    src/SqliteDatabase.cpp
    src/SqliteProjectDataSource.cpp
    src/SqliteFolderDataSource.cpp
    src/SqliteNoteDataSource.cpp
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${PLOTTER_REPOSITORIES_DIR}/include
    ${PLOTTER_DTOS_DIR}/include
    ${PLOTTER_SQLITE_DTOS_DIR}/include
)

# Link libraries  
target_link_libraries(${PROJECT_NAME} 
    PUBLIC 
        SQLite::SQLite3
)

# Install headers
install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

# Install library
install(TARGETS ${PROJECT_NAME}
    EXPORT PlotterSqliteDataSourceTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

# Examples
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
