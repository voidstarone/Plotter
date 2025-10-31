# Examples CMakeLists.txt

# SQLite Demo
add_executable(sqlite_demo
    sqlite_demo.cpp
)

target_link_libraries(sqlite_demo
    PRIVATE
        PlotterSqliteDataSource
        NoteTaker
        SQLite::SQLite3
)

# Copy to build directory for easy execution
install(TARGETS sqlite_demo
    RUNTIME DESTINATION bin
)
