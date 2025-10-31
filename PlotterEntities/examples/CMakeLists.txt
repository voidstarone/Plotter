# Example CMakeLists.txt for using NoteTaker as a library

cmake_minimum_required(VERSION 3.16)
project(NoteTakerExamples VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find NoteTaker library (assumes it's been built and installed)
# If using from source, adjust the paths accordingly
find_library(NOTETAKER_LIB NoteTaker 
    HINTS ../build/lib
    REQUIRED
)

# Include directories
include_directories(../include)

# Create example executable
add_executable(library_usage library_usage.cpp)

# Link with NoteTaker library
target_link_libraries(library_usage ${NOTETAKER_LIB})

# Alternative: If building from source without installing
# target_link_libraries(library_usage NoteTaker)