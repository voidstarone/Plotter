cmake_minimum_required(VERSION 3.15)
project(PlotterDataSourceRouter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This is a header-only library
add_library(plotter_datasource_router INTERFACE)

target_include_directories(plotter_datasource_router INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# For compatibility, also set the PlotterRepositories include path
# since SimpleDataSourceRouter depends on DataSourceRouter from PlotterRepositories
target_include_directories(plotter_datasource_router INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/../PlotterRepositories/include
)
