cmake_minimum_required(VERSION 3.20)
project(PlotterFilesystemDataSource VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Accept configuration options (passed from build system)
set(PLOTTER_ENABLE_SQLITE ON CACHE BOOL "Enable SQLite data source support")
set(PLOTTER_ENABLE_FILESYSTEM ON CACHE BOOL "Enable Filesystem data source support")

# Find jsoncpp - try pkg-config first, then fallback to manual search
find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(JSONCPP jsoncpp)
endif()

# If pkg-config didn't work, try to find jsoncpp manually
if(NOT JSONCPP_FOUND)
    # Try common Homebrew paths
    set(JSONCPP_INCLUDE_DIRS "/opt/homebrew/include" "/usr/local/include")
    find_library(JSONCPP_LIBRARY
        NAMES jsoncpp
        PATHS /opt/homebrew/lib /usr/local/lib
    )
    if(JSONCPP_LIBRARY)
        set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY})
    else()
        set(JSONCPP_LIBRARIES "jsoncpp")
    endif()
    message(STATUS "pkg-config not found or jsoncpp not found via pkg-config, using fallback paths")
endif()

# Add the library
add_library(PlotterFilesystemDataSource
    src/FilesystemProjectDataSource.cpp
    src/FilesystemFolderDataSource.cpp
    src/FilesystemNoteDataSource.cpp
    src/FilesystemNoteStorage.cpp
)

# Include directories
target_include_directories(PlotterFilesystemDataSource
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
        ${JSONCPP_INCLUDE_DIRS}
)

# Link dependencies
target_include_directories(PlotterFilesystemDataSource
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../PlotterDTOs/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../PlotterFilesystemDTOs/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../PlotterEntities/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../PlotterRepositories/include
)

target_link_libraries(PlotterFilesystemDataSource
    PUBLIC
        PlotterFilesystemDTOs
        ${JSONCPP_LIBRARIES}
)

# Installation rules
install(TARGETS PlotterFilesystemDataSource
    EXPORT PlotterFilesystemDataSourceTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
)

# Only add tests if explicitly enabled and jsoncpp library was found
if(BUILD_TESTING AND JSONCPP_LIBRARY)
    enable_testing()
    add_subdirectory(tests)
endif()
