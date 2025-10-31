cmake_minimum_required(VERSION 3.16)
project(PlotterUseCases VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the PlotterEntities library
set(PLOTTER_ENTITIES_DIR "${CMAKE_SOURCE_DIR}/../PlotterEntities")

# Check if PlotterEntities exists
if(NOT EXISTS "${PLOTTER_ENTITIES_DIR}")
    message(FATAL_ERROR "PlotterEntities directory not found at ${PLOTTER_ENTITIES_DIR}")
endif()

# Include directories
include_directories(include)
include_directories("${PLOTTER_ENTITIES_DIR}/include")

# Use Case source files
set(USECASE_SOURCES
    # Project Management Use Cases  
    src/usecases/CreateProjectUseCase.cpp
    src/usecases/GetProjectUseCase.cpp
    src/usecases/ListProjectsUseCase.cpp
    # src/usecases/DeleteProjectUseCase.cpp  # Has compilation errors
    
    # Folder Management Use Cases
    src/usecases/CreateFolderUseCase.cpp
    # src/usecases/MoveFolderUseCase.cpp  # Has compilation errors
    
    # Note Management Use Cases
    src/usecases/CreateNoteUseCase.cpp
    # src/usecases/MoveNoteUseCase.cpp  # Has compilation errors
    src/usecases/GetNoteContentUseCase.cpp
    
    # Common base classes (header-only, included for IDE support)
    # include/usecases/UseCaseCommon.h
    # include/usecases/BaseUseCase.h
)

# Build PlotterEntities library first
add_subdirectory("${PLOTTER_ENTITIES_DIR}" entities)

# Create the Use Cases library
add_library(${PROJECT_NAME} STATIC ${USECASE_SOURCES})

# Link with PlotterEntities
target_link_libraries(${PROJECT_NAME} NoteTaker)

# Set library output directory
set_target_properties(${PROJECT_NAME} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Install targets
install(TARGETS ${PROJECT_NAME}
    ARCHIVE DESTINATION lib
)

install(DIRECTORY include/
    DESTINATION include/PlotterUseCases
    FILES_MATCHING PATTERN "*.h"
)

# Optional: Create a demo executable if requested
option(BUILD_EXAMPLES "Build example applications" ON)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Optional: Build tests if requested
option(BUILD_TESTS "Build test suite" OFF)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()