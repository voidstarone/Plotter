# Tests for PlotterSqliteDTOs

add_executable(test_sqlite_dtos test_sqlite_dtos.cpp)

target_link_libraries(test_sqlite_dtos PRIVATE PlotterSqliteDTOs)

target_include_directories(test_sqlite_dtos PRIVATE
    ${PLOTTER_DTOS_DIR}/include
)

# Add as a test
enable_testing()
add_test(NAME SqliteDTOTests COMMAND test_sqlite_dtos)
