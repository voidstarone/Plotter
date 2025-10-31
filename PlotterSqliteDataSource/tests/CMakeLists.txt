# Tests for PlotterSqliteDataSource

# Build PlotterSqliteDTOs
add_subdirectory(${PLOTTER_SQLITE_DTOS_DIR} sqlite_dtos)

add_executable(test_datasource test_datasource.cpp)

target_link_libraries(test_datasource PRIVATE 
    PlotterSqliteDataSource
    PlotterSqliteDTOs
    SQLite::SQLite3
)

# Add as a test
enable_testing()
add_test(NAME DataSourceTests COMMAND test_datasource)
