cmake_minimum_required(VERSION 3.20)

# Simple structural test - doesn't require full entity/repository linkage
add_executable(test_mappers test_mappers.cpp)

# Link against PlotterFilesystemDTOs library
add_library(PlotterFilesystemDTOs STATIC IMPORTED)
set_target_properties(PlotterFilesystemDTOs PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../PlotterFilesystemDTOs/build/libPlotterFilesystemDTOs.a
)

target_link_libraries(test_mappers
    PRIVATE
        PlotterFilesystemMappers
        PlotterFilesystemDTOs
)

target_include_directories(test_mappers
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../PlotterFilesystemDTOs/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../PlotterDTOs/include
)

# Add as a test
add_test(NAME MapperTests COMMAND test_mappers)
