#!/bin/bash

# Plotter Backend Build Script
# Builds all backend modules with optional data source selection

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

# Get the script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Default options
BUILD_SQLITE=true
BUILD_FILESYSTEM=true
BUILD_TESTS=true
CLEAN_BUILD=false
VERBOSE=false
PARALLEL_JOBS=$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

# CMake options based on data source selection
CMAKE_OPTS=""

# Parse command line arguments
print_usage() {
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Build the Plotter backend (all modules except CLI)"
    echo ""
    echo "Options:"
    echo "  -s, --sqlite-only       Build only SQLite data source modules"
    echo "  -f, --filesystem-only   Build only Filesystem data source modules"
    echo "  -b, --both              Build both data sources (default)"
    echo "  -c, --clean             Clean build directories before building"
    echo "  --no-tests              Skip building and running tests"
    echo "  -v, --verbose           Show detailed build output"
    echo "  -j, --jobs N            Number of parallel jobs (default: $PARALLEL_JOBS)"
    echo "  -h, --help              Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                      # Build everything (default)"
    echo "  $0 --sqlite-only        # Build only core + SQLite modules"
    echo "  $0 --filesystem-only    # Build only core + Filesystem modules"
    echo "  $0 -c -v                # Clean build with verbose output"
    echo "  $0 --no-tests -j 8      # Build without tests, 8 parallel jobs"
}

while [[ $# -gt 0 ]]; do
    case $1 in
        -s|--sqlite-only)
            BUILD_SQLITE=true
            BUILD_FILESYSTEM=false
            shift
            ;;
        -f|--filesystem-only)
            BUILD_SQLITE=false
            BUILD_FILESYSTEM=true
            shift
            ;;
        -b|--both)
            BUILD_SQLITE=true
            BUILD_FILESYSTEM=true
            shift
            ;;
        -c|--clean)
            CLEAN_BUILD=true
            shift
            ;;
        --no-tests)
            BUILD_TESTS=false
            shift
            ;;
        -v|--verbose)
            VERBOSE=true
            shift
            ;;
        -j|--jobs)
            PARALLEL_JOBS="$2"
            shift 2
            ;;
        -h|--help)
            print_usage
            exit 0
            ;;
        *)
            echo -e "${RED}Error: Unknown option $1${NC}"
            print_usage
            exit 1
            ;;
    esac
done

# Set CMake options based on data source selection
if [ "$BUILD_SQLITE" = true ]; then
    CMAKE_OPTS="$CMAKE_OPTS -DPLOTTER_ENABLE_SQLITE=ON"
else
    CMAKE_OPTS="$CMAKE_OPTS -DPLOTTER_ENABLE_SQLITE=OFF"
fi

if [ "$BUILD_FILESYSTEM" = true ]; then
    CMAKE_OPTS="$CMAKE_OPTS -DPLOTTER_ENABLE_FILESYSTEM=ON"
else
    CMAKE_OPTS="$CMAKE_OPTS -DPLOTTER_ENABLE_FILESYSTEM=OFF"
fi

# Print build configuration
echo -e "${CYAN}================================================${NC}"
echo -e "${CYAN}Plotter Backend Build${NC}"
echo -e "${CYAN}================================================${NC}"
echo ""
echo -e "${BLUE}Build Configuration:${NC}"
echo -e "  SQLite modules:     ${GREEN}$([ "$BUILD_SQLITE" = true ] && echo "YES" || echo "NO")${NC}"
echo -e "  Filesystem modules: ${GREEN}$([ "$BUILD_FILESYSTEM" = true ] && echo "YES" || echo "NO")${NC}"
echo -e "  Build tests:        ${GREEN}$([ "$BUILD_TESTS" = true ] && echo "YES" || echo "NO")${NC}"
echo -e "  Clean build:        ${GREEN}$([ "$CLEAN_BUILD" = true ] && echo "YES" || echo "NO")${NC}"
echo -e "  Parallel jobs:      ${GREEN}${PARALLEL_JOBS}${NC}"
echo ""

# Check dependencies
check_dependency() {
    local dep=$1
    local install_cmd=$2

    if ! command -v "$dep" &> /dev/null && ! pkg-config --exists "$dep" 2>/dev/null; then
        echo -e "${RED}✗ $dep not found!${NC}"
        echo -e "  Install with: ${YELLOW}$install_cmd${NC}"
        return 1
    fi
    return 0
}

echo -e "${BLUE}Checking dependencies...${NC}"

# Check CMake
if ! check_dependency cmake "brew install cmake"; then
    exit 1
fi
echo -e "${GREEN}✓ cmake${NC}"

# Check for jsoncpp if building filesystem
if [ "$BUILD_FILESYSTEM" = true ]; then
    # Try to find pkg-config in common locations
    PKG_CONFIG=""
    if command -v pkg-config &> /dev/null; then
        PKG_CONFIG="pkg-config"
    elif [ -f /opt/homebrew/bin/pkg-config ]; then
        PKG_CONFIG="/opt/homebrew/bin/pkg-config"
    elif [ -f /usr/local/bin/pkg-config ]; then
        PKG_CONFIG="/usr/local/bin/pkg-config"
    fi

    if [ -z "$PKG_CONFIG" ]; then
        echo -e "${YELLOW}⚠ pkg-config not found, checking for jsoncpp manually...${NC}"
        # Check if jsoncpp is installed via Homebrew
        if brew list jsoncpp &> /dev/null; then
            echo -e "${GREEN}✓ jsoncpp (found via Homebrew)${NC}"
        else
            echo -e "${RED}✗ jsoncpp not found!${NC}"
            echo -e "  Install with: ${YELLOW}brew install jsoncpp${NC}"
            exit 1
        fi
    else
        if $PKG_CONFIG --exists jsoncpp; then
            echo -e "${GREEN}✓ jsoncpp${NC}"
        else
            echo -e "${RED}✗ jsoncpp not found!${NC}"
            echo -e "  Install with: ${YELLOW}brew install jsoncpp${NC}"
            exit 1
        fi
    fi
fi

echo ""

# Function to build a module
build_module() {
    local module_name=$1
    local module_path="$SCRIPT_DIR/$2"
    local skip_tests=${3:-false}

    if [ ! -d "$module_path" ]; then
        echo -e "${YELLOW}⊘ Skipping $module_name (directory not found)${NC}"
        return 0
    fi

    echo -e "${BLUE}Building $module_name...${NC}"

    cd "$module_path"

    # Create build directory
    mkdir -p build
    cd build

    # Clean if requested
    if [ "$CLEAN_BUILD" = true ]; then
        echo "  Cleaning..."
        find . -mindepth 1 -delete
    fi

    # Configure with CMake
    echo "  Configuring..."
    if [ "$VERBOSE" = true ]; then
        cmake .. $CMAKE_OPTS
    else
        cmake .. $CMAKE_OPTS > /dev/null 2>&1
    fi

    if [ $? -ne 0 ]; then
        echo -e "${RED}✗ CMake configuration failed for $module_name${NC}"
        return 1
    fi

    # Build
    echo "  Compiling..."
    if [ "$VERBOSE" = true ]; then
        make -j"$PARALLEL_JOBS"
    else
        make -j"$PARALLEL_JOBS" > /dev/null 2>&1
    fi

    if [ $? -ne 0 ]; then
        echo -e "${RED}✗ Build failed for $module_name${NC}"
        return 1
    fi

    echo -e "${GREEN}✓ $module_name built successfully${NC}"

    # Run tests if enabled
    if [ "$BUILD_TESTS" = true ] && [ "$skip_tests" = false ]; then
        echo "  Running tests..."
        if [ "$VERBOSE" = true ]; then
            make test
        else
            if make test > /dev/null 2>&1; then
                echo -e "${GREEN}✓ Tests passed${NC}"
            else
                echo -e "${YELLOW}⚠ Tests failed (continuing anyway)${NC}"
            fi
        fi
    fi

    cd "$SCRIPT_DIR"
    echo ""
}

# Track build results
declare -a BUILT_MODULES=()
declare -a FAILED_MODULES=()

build_and_track() {
    local module_name=$1
    if build_module "$@"; then
        BUILT_MODULES+=("$module_name")
        return 0
    else
        FAILED_MODULES+=("$module_name")
        return 1
    fi
}

# Build order
echo -e "${CYAN}================================================${NC}"
echo -e "${CYAN}Building Core Modules${NC}"
echo -e "${CYAN}================================================${NC}"
echo ""

# Core modules (always built)
build_and_track "PlotterDTOs" "PlotterDTOs" true || true
build_and_track "PlotterLogger" "PlotterLogger" || true

echo -e "${CYAN}================================================${NC}"
echo -e "${CYAN}Building Data Source Modules${NC}"
echo -e "${CYAN}================================================${NC}"
echo ""

# SQLite modules
if [ "$BUILD_SQLITE" = true ]; then
    echo -e "${BLUE}SQLite Data Source:${NC}"
    build_and_track "PlotterSqliteDTOs" "PlotterSqliteDTOs" || true
    build_and_track "PlotterSqliteMappers" "PlotterSqliteMappers" || true
    # Note: PlotterSqliteDataSource folder is empty, skipping
    echo ""
fi

# Filesystem modules
if [ "$BUILD_FILESYSTEM" = true ]; then
    echo -e "${BLUE}Filesystem Data Source:${NC}"
    build_and_track "PlotterFilesystemDTOs" "PlotterFilesystemDTOs" || true
    build_and_track "PlotterFilesystemMappers" "PlotterFilesystemMappers" || true
    build_and_track "PlotterFilesystemDataSource" "PlotterFilesystemDataSource" || true
    echo ""
fi

# Build router (if directory exists)
if [ -d "$SCRIPT_DIR/PlotterDataSourceRouter" ]; then
    echo -e "${CYAN}================================================${NC}"
    echo -e "${CYAN}Building Router${NC}"
    echo -e "${CYAN}================================================${NC}"
    echo ""
    build_and_track "PlotterDataSourceRouter" "PlotterDataSourceRouter" true || true
fi

# Print summary
echo -e "${CYAN}================================================${NC}"
echo -e "${CYAN}Build Summary${NC}"
echo -e "${CYAN}================================================${NC}"
echo ""

if [ ${#BUILT_MODULES[@]} -gt 0 ]; then
    echo -e "${GREEN}Successfully built (${#BUILT_MODULES[@]} modules):${NC}"
    for module in "${BUILT_MODULES[@]}"; do
        echo -e "  ${GREEN}✓${NC} $module"
    done
    echo ""
fi

if [ ${#FAILED_MODULES[@]} -gt 0 ]; then
    echo -e "${RED}Failed to build (${#FAILED_MODULES[@]} modules):${NC}"
    for module in "${FAILED_MODULES[@]}"; do
        echo -e "  ${RED}✗${NC} $module"
    done
    echo ""
    exit 1
fi

echo -e "${GREEN}================================================${NC}"
echo -e "${GREEN}All backend modules built successfully!${NC}"
echo -e "${GREEN}================================================${NC}"
echo ""

# Print next steps
echo -e "${BLUE}Next steps:${NC}"
if [ "$BUILD_SQLITE" = true ] && [ "$BUILD_FILESYSTEM" = true ]; then
    echo "  • Both SQLite and Filesystem backends are ready"
elif [ "$BUILD_SQLITE" = true ]; then
    echo "  • SQLite backend is ready"
    echo "  • To add Filesystem support, run: $0 --filesystem-only"
elif [ "$BUILD_FILESYSTEM" = true ]; then
    echo "  • Filesystem backend is ready"
    echo "  • To add SQLite support, run: $0 --sqlite-only"
fi

echo "  • Integrate with CLI by linking against the built libraries"
echo "  • Libraries are in: <module>/build/"
echo ""

if [ "$BUILD_FILESYSTEM" = true ]; then
    echo -e "${BLUE}Filesystem backend example structure:${NC}"
    echo "  ~/PlotterData/"
    echo "    ├── MyProject/"
    echo "    │   ├── .plotter_project"
    echo "    │   ├── Folder1/"
    echo "    │   │   ├── .plotter_folder"
    echo "    │   │   ├── note.md"
    echo "    │   │   └── note.md.plotter_meta"
    echo "    │   └── Folder2/"
    echo "    │       └── .plotter_folder"
    echo ""
fi

echo -e "${GREEN}Build complete!${NC}"
