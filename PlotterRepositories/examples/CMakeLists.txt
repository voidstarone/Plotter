# Examples for PlotterRepositories

# Find and link PlotterEntities library
find_library(NOTETAKER_LIB NoteTaker PATHS ${PLOTTER_ENTITIES_DIR}/build/lib REQUIRED)

# Build the simple demo
add_executable(simple_demo simple_demo.cpp)

# Link with the repository interface library and entities
target_link_libraries(simple_demo PRIVATE
    PlotterRepositories
    ${NOTETAKER_LIB}
)

# Include test mocks directory for the demo
target_include_directories(simple_demo PRIVATE
    ${CMAKE_SOURCE_DIR}/tests
)

message(STATUS "Building simple repository demonstration")
