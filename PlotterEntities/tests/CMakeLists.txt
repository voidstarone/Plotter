# Test for PlotterEntities

add_executable(test_entities test_entities.cpp)

target_link_libraries(test_entities PRIVATE NoteTaker)

# Add as a test
enable_testing()
add_test(NAME EntityTests COMMAND test_entities)
