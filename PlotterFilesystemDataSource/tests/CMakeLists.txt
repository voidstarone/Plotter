cmake_minimum_required(VERSION 3.20)

# Find jsoncpp - make it optional like in the main CMakeLists
find_package(PkgConfig)
if(PkgConfig_FOUND)
    pkg_check_modules(JSONCPP jsoncpp)
endif()

# If pkg-config didn't work, use fallback paths and find the actual library
if(NOT JSONCPP_FOUND)
    set(JSONCPP_INCLUDE_DIRS "/opt/homebrew/include" "/usr/local/include")
    # Find the actual jsoncpp library file
    find_library(JSONCPP_LIBRARY
        NAMES jsoncpp
        PATHS /opt/homebrew/lib /usr/local/lib
    )
    if(JSONCPP_LIBRARY)
        set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY})
    else()
        # Fallback to library name
        set(JSONCPP_LIBRARIES "jsoncpp")
    endif()
endif()

# Import PlotterFilesystemDTOs library
add_library(PlotterFilesystemDTOs STATIC IMPORTED)
set_target_properties(PlotterFilesystemDTOs PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../PlotterFilesystemDTOs/build/libPlotterFilesystemDTOs.a
)

# Add test executable
add_executable(test_filesystem_datasource
    test_datasource.cpp
)

# Link against the library
target_link_libraries(test_filesystem_datasource
    PRIVATE
        PlotterFilesystemDataSource
        PlotterFilesystemDTOs
        ${JSONCPP_LIBRARIES}
)

target_include_directories(test_filesystem_datasource
    PRIVATE
        ${JSONCPP_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/../../PlotterFilesystemDTOs/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../PlotterDTOs/include
)

# Add test
add_test(NAME FilesystemDataSourceTest COMMAND test_filesystem_datasource)
