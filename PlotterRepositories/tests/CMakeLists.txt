# Tests for PlotterRepositories

# Build the unit test
add_executable(test_repository test_repository.cpp)

# Link with the repository interface library
target_link_libraries(test_repository
    PlotterRepositories
    PlotterUseCases
    NoteTaker
)

# Include mocks directory
target_include_directories(test_repository PRIVATE
    ${CMAKE_SOURCE_DIR}/tests
)

# Add as a test
add_test(NAME RepositoryTests COMMAND test_repository)

message(STATUS "Building repository tests with mock implementations")
