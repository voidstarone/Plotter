cmake_minimum_required(VERSION 3.16)
project(NoteTaker VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Library source files (excluding main.cpp)
set(LIBRARY_SOURCES
    src/FileItem.cpp
    src/Project.cpp
    src/Folder.cpp
    src/Note.cpp
    src/FileSystemNoteStorage.cpp
)

# Create static library
add_library(${PROJECT_NAME} STATIC ${LIBRARY_SOURCES})

# Set library output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Create demo executable (optional)
option(BUILD_EXAMPLES "Build examples" ON)
if(BUILD_EXAMPLES)
    add_executable(${PROJECT_NAME}_demo src/main.cpp)

    # Link demo with library
    target_link_libraries(${PROJECT_NAME}_demo ${PROJECT_NAME})

    # Set demo output directory
    set_target_properties(${PROJECT_NAME}_demo PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
endif()

# Tests
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Install targets
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include/NoteTaker
    FILES_MATCHING PATTERN "*.h"
)

if(BUILD_EXAMPLES)
    install(TARGETS ${PROJECT_NAME}_demo
        RUNTIME DESTINATION bin
    )
endif()
