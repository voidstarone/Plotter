#!/bin/bash

# Master integration test script for Plotter backends
# Runs integration tests for both SQLite and Filesystem data sources

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo -e "${CYAN}================================================${NC}"
echo -e "${CYAN}Plotter Backend Integration Tests${NC}"
echo -e "${CYAN}================================================${NC}"
echo ""

# Check for Python 3
if ! command -v python3 &> /dev/null; then
    echo -e "${RED}✗ Python 3 not found${NC}"
    echo "  Please install Python 3"
    exit 1
fi

echo -e "${GREEN}✓ Python 3 found${NC}"

# Check for send2trash (optional but recommended)
if python3 -c "import send2trash" 2>/dev/null; then
    echo -e "${GREEN}✓ send2trash installed (will use trash)${NC}"
else
    echo -e "${YELLOW}⚠ send2trash not installed (will permanently delete)${NC}"
    echo -e "  Install with: ${BLUE}pip3 install send2trash${NC}"
fi

echo ""

# Parse arguments
RUN_SQLITE=true
RUN_FILESYSTEM=true

while [[ $# -gt 0 ]]; do
    case $1 in
        --sqlite-only)
            RUN_SQLITE=true
            RUN_FILESYSTEM=false
            shift
            ;;
        --filesystem-only)
            RUN_SQLITE=false
            RUN_FILESYSTEM=true
            shift
            ;;
        -h|--help)
            echo "Usage: $0 [OPTIONS]"
            echo ""
            echo "Options:"
            echo "  --sqlite-only       Run only SQLite integration tests"
            echo "  --filesystem-only   Run only Filesystem integration tests"
            echo "  -h, --help          Show this help message"
            exit 0
            ;;
        *)
            echo -e "${RED}Unknown option: $1${NC}"
            exit 1
            ;;
    esac
done

TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Run SQLite integration test
if [ "$RUN_SQLITE" = true ]; then
    echo -e "${CYAN}Running SQLite integration test...${NC}"
    if python3 "$SCRIPT_DIR/integration_test_sqlite.py"; then
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo ""
fi

# Run Filesystem integration test
if [ "$RUN_FILESYSTEM" = true ]; then
    echo -e "${CYAN}Running Filesystem integration test...${NC}"
    if python3 "$SCRIPT_DIR/integration_test_filesystem.py"; then
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    echo ""
fi

# Print summary
echo -e "${CYAN}================================================${NC}"
echo -e "${CYAN}Integration Test Summary${NC}"
echo -e "${CYAN}================================================${NC}"
echo ""
echo -e "Total tests:  ${BLUE}${TOTAL_TESTS}${NC}"
echo -e "Passed:       ${GREEN}${PASSED_TESTS}${NC}"
echo -e "Failed:       ${RED}${FAILED_TESTS}${NC}"
echo ""

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}✅ All integration tests passed!${NC}"
    exit 0
else
    echo -e "${RED}❌ Some integration tests failed${NC}"
    exit 1
fi

# Run all Plotter integration tests

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "======================================================================="
echo "  Plotter Integration Test Suite"
echo "======================================================================="
echo ""

# Track results
TOTAL_TESTS=0
PASSED_TESTS=0

# Function to run a test and track results
run_test() {
    local test_name="$1"
    local test_script="$2"

    echo "-----------------------------------------------------------------------"
    echo "Running: $test_name"
    echo "-----------------------------------------------------------------------"

    TOTAL_TESTS=$((TOTAL_TESTS + 1))

    if python3 "$test_script"; then
        PASSED_TESTS=$((PASSED_TESTS + 1))
        echo "✅ $test_name PASSED"
    else
        echo "❌ $test_name FAILED"
    fi
    echo ""
}

# Run data source tests
echo "=== DATA SOURCE TESTS ==="
echo ""
run_test "SQLite Data Source Tests" "integration_test_sqlite.py"
run_test "Filesystem Data Source Tests" "integration_test_filesystem.py"

# Run use case integration tests
echo ""
echo "=== USE CASE INTEGRATION TESTS ==="
echo ""
run_test "Use Cases with SQLite Backend" "integration_test_usecases_sqlite.py"
run_test "Use Cases with Filesystem Backend" "integration_test_usecases_filesystem.py"

# Print final summary
echo "======================================================================="
echo "  TEST SUMMARY"
echo "======================================================================="
echo "  Total Tests: $TOTAL_TESTS"
echo "  Passed:      $PASSED_TESTS"
echo "  Failed:      $((TOTAL_TESTS - PASSED_TESTS))"
echo ""

if [ $PASSED_TESTS -eq $TOTAL_TESTS ]; then
    echo "  ✅ ALL TESTS PASSED!"
    echo "======================================================================="
    exit 0
else
    echo "  ❌ SOME TESTS FAILED"
    echo "======================================================================="
    exit 1
fi
