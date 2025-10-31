#!/bin/bash
# Master test script - Runs ALL tests (C++ unit tests + Python integration tests)

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/.." && pwd)"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}=======================================================================${NC}"
echo -e "${CYAN}  Plotter Complete Test Suite${NC}"
echo -e "${CYAN}  Running all C++ unit tests and Python integration tests${NC}"
echo -e "${CYAN}=======================================================================${NC}"
echo ""

TOTAL_SUITES=0
PASSED_SUITES=0

# Function to run a test suite
run_test_suite() {
    local suite_name="$1"
    local test_command="$2"

    echo -e "${BLUE}-----------------------------------------------------------------------${NC}"
    echo -e "${BLUE}Running: $suite_name${NC}"
    echo -e "${BLUE}-----------------------------------------------------------------------${NC}"

    TOTAL_SUITES=$((TOTAL_SUITES + 1))

    if eval "$test_command"; then
        PASSED_SUITES=$((PASSED_SUITES + 1))
        echo -e "${GREEN}✅ $suite_name PASSED${NC}"
    else
        echo -e "${RED}❌ $suite_name FAILED${NC}"
    fi
    echo ""
}

# ============================================================================
# Part 1: C++ Unit Tests
# ============================================================================

echo -e "${CYAN}=== PART 1: C++ UNIT TESTS ===${NC}"
echo ""

# Test Entities
if [ -f "$PROJECT_ROOT/PlotterEntities/build/tests/EntityTests" ]; then
    run_test_suite "Entity Unit Tests" "$PROJECT_ROOT/PlotterEntities/build/tests/EntityTests"
else
    echo -e "${YELLOW}⚠ Entity tests not built (run: cd PlotterEntities/build && cmake .. && make)${NC}"
    echo ""
fi

# Test Use Cases
if [ -f "$PROJECT_ROOT/PlotterUseCases/build/tests/UseCaseTests" ]; then
    run_test_suite "Use Case Unit Tests" "$PROJECT_ROOT/PlotterUseCases/build/tests/UseCaseTests"
else
    echo -e "${YELLOW}⚠ Use case tests not built (run: cd PlotterUseCases/build && cmake .. && make)${NC}"
    echo ""
fi

# Test SQLite Data Source
if [ -f "$PROJECT_ROOT/PlotterSqliteDataSource/build/tests/SqliteDataSourceTests" ]; then
    run_test_suite "SQLite Data Source Tests" "$PROJECT_ROOT/PlotterSqliteDataSource/build/tests/SqliteDataSourceTests"
else
    echo -e "${YELLOW}⚠ SQLite data source tests not built${NC}"
    echo ""
fi

# Test Filesystem Data Source
if [ -f "$PROJECT_ROOT/PlotterFilesystemDataSource/build/tests/FilesystemDataSourceTests" ]; then
    run_test_suite "Filesystem Data Source Tests" "$PROJECT_ROOT/PlotterFilesystemDataSource/build/tests/FilesystemDataSourceTests"
else
    echo -e "${YELLOW}⚠ Filesystem data source tests not built${NC}"
    echo ""
fi

# Test SQLite Mappers
if [ -f "$PROJECT_ROOT/PlotterSqliteMappers/build/tests/SqliteMapperTests" ]; then
    run_test_suite "SQLite Mapper Tests" "$PROJECT_ROOT/PlotterSqliteMappers/build/tests/SqliteMapperTests"
else
    echo -e "${YELLOW}⚠ SQLite mapper tests not built${NC}"
    echo ""
fi

# Test Filesystem Mappers
if [ -f "$PROJECT_ROOT/PlotterFilesystemMappers/build/tests/FilesystemMapperTests" ]; then
    run_test_suite "Filesystem Mapper Tests" "$PROJECT_ROOT/PlotterFilesystemMappers/build/tests/FilesystemMapperTests"
else
    echo -e "${YELLOW}⚠ Filesystem mapper tests not built${NC}"
    echo ""
fi

# Test Repositories
if [ -f "$PROJECT_ROOT/PlotterRepositories/build/tests/RepositoryTests" ]; then
    run_test_suite "Repository Tests" "$PROJECT_ROOT/PlotterRepositories/build/tests/RepositoryTests"
else
    echo -e "${YELLOW}⚠ Repository tests not built${NC}"
    echo ""
fi

# ============================================================================
# Part 2: Python Integration Tests
# ============================================================================

echo ""
echo -e "${CYAN}=== PART 2: PYTHON INTEGRATION TESTS ===${NC}"
echo ""

# Check for Python 3
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}✗ Python 3 not found - skipping integration tests${NC}"
else
    run_test_suite "SQLite Data Source Integration" "python3 $SCRIPT_DIR/integration_test_sqlite.py"
    run_test_suite "Filesystem Data Source Integration" "python3 $SCRIPT_DIR/integration_test_filesystem.py"
    run_test_suite "Use Cases with SQLite Backend" "python3 $SCRIPT_DIR/integration_test_usecases_sqlite.py"
    run_test_suite "Use Cases with Filesystem Backend" "python3 $SCRIPT_DIR/integration_test_usecases_filesystem.py"
fi

# ============================================================================
# Final Summary
# ============================================================================

echo ""
echo -e "${CYAN}=======================================================================${NC}"
echo -e "${CYAN}  FINAL TEST SUMMARY${NC}"
echo -e "${CYAN}=======================================================================${NC}"
echo ""
echo -e "  Total Test Suites: ${BLUE}$TOTAL_SUITES${NC}"
echo -e "  Passed:            ${GREEN}$PASSED_SUITES${NC}"
echo -e "  Failed:            ${RED}$((TOTAL_SUITES - PASSED_SUITES))${NC}"
echo ""

if [ $PASSED_SUITES -eq $TOTAL_SUITES ]; then
    echo -e "  ${GREEN}✅ ALL TESTS PASSED!${NC}"
    echo -e "${CYAN}=======================================================================${NC}"
    exit 0
else
    echo -e "  ${RED}❌ SOME TESTS FAILED${NC}"
    echo -e "${CYAN}=======================================================================${NC}"
    exit 1
fi

