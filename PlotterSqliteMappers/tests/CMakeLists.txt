# Tests for PlotterSqliteMappers

# Build PlotterEntities first
add_subdirectory(${PLOTTER_ENTITIES_DIR} entities)

# Build PlotterSqliteDTOs
add_subdirectory(${PLOTTER_SQLITE_DTOS_DIR} sqlite_dtos)

add_executable(test_mappers test_mappers.cpp)

target_link_libraries(test_mappers PRIVATE 
    PlotterSqliteMappers
    PlotterSqliteDTOs
    NoteTaker
)

# Add as a test
enable_testing()
add_test(NAME MapperTests COMMAND test_mappers)
