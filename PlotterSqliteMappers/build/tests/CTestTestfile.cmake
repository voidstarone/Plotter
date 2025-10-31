# CMake generated Testfile for 
# Source directory: /Users/tlee/Developer/Plotter/PlotterSqliteMappers/tests
# Build directory: /Users/tlee/Developer/Plotter/PlotterSqliteMappers/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(MapperTests "/Users/tlee/Developer/Plotter/PlotterSqliteMappers/build/tests/test_mappers")
set_tests_properties(MapperTests PROPERTIES  _BACKTRACE_TRIPLES "/Users/tlee/Developer/Plotter/PlotterSqliteMappers/tests/CMakeLists.txt;19;add_test;/Users/tlee/Developer/Plotter/PlotterSqliteMappers/tests/CMakeLists.txt;0;")
subdirs("entities")
subdirs("sqlite_dtos")
