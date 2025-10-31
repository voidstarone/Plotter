# Examples CMakeLists.txt

# Include test mocks for repository implementations
include_directories("${CMAKE_SOURCE_DIR}/tests/mocks")

# Mock repository implementations (for demonstration only)
set(MOCK_SOURCES
    ../tests/mocks/InMemoryProjectRepository.cpp
    ../tests/mocks/InMemoryFolderRepository.cpp
    ../tests/mocks/InMemoryNoteRepository.cpp
)

# Create basic demo executable
add_executable(use_cases_demo 
    demo_application.cpp
    ${MOCK_SOURCES}
)

# Create robust demo executable  
add_executable(robust_demo
    robust_demo.cpp
    ${MOCK_SOURCES}
)

# Link with libraries
target_link_libraries(use_cases_demo 
    ${PROJECT_NAME}
    NoteTaker
)

target_link_libraries(robust_demo
    ${PROJECT_NAME}
    NoteTaker
)

# Set demo output directory
set_target_properties(use_cases_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

set_target_properties(robust_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Install demos
install(TARGETS use_cases_demo robust_demo
    RUNTIME DESTINATION bin
)