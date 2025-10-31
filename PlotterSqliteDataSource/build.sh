#!/bin/bash

# Build script for PlotterSqliteDataSource

set -e  # Exit on error

echo "=== Building PlotterSqliteDataSource ==="
echo ""

# Create build directory
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

# Configure
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

# Build
echo "Building..."
cmake --build . -j$(sysctl -n hw.ncpu 2>/dev/null || nproc 2>/dev/null || echo 4)

echo ""
echo "=== Build complete! ==="
echo ""
echo "Run the example:"
echo "  cd build/examples"
echo "  ./sqlite_demo"
echo ""
