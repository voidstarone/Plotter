cmake_minimum_required(VERSION 3.20)

# Simple structural test - doesn't require full entity/repository linkage
add_executable(test_mappers test_mappers.cpp)

# Link against PlotterSqliteDTOs library
add_library(PlotterSqliteDTOs STATIC IMPORTED)
set_target_properties(PlotterSqliteDTOs PROPERTIES
    IMPORTED_LOCATION ${CMAKE_CURRENT_SOURCE_DIR}/../../PlotterSqliteDTOs/build/libPlotterSqliteDTOs.a
)

target_link_libraries(test_mappers
    PRIVATE
        PlotterSqliteMappers
        PlotterSqliteDTOs
)

target_include_directories(test_mappers
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../PlotterSqliteDTOs/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../PlotterDTOs/include
)

# Add as a test
add_test(NAME MapperTests COMMAND test_mappers)
