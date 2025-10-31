#!/bin/bash
set -e

echo "🔨 Building Plotter C++ Libraries..."

# Build order matters due to dependencies
LIBS=(
    "PlotterLogger"
    "PlotterEntities" 
    "PlotterDTOs"
    "PlotterSqliteDTOs"
    "PlotterRepositories"
    "PlotterSqliteMappers"
    "PlotterSqliteDataSource"
    "PlotterUseCases"
)

for lib in "${LIBS[@]}"; do
    echo "Building $lib..."
    cd "$lib"
    mkdir -p build
    cd build
    cmake .. -DBUILD_EXAMPLES=OFF -DBUILD_TESTS=OFF
    make -j$(sysctl -n hw.ncpu)
    cd ../..
done

echo "✅ All C++ libraries built"
echo ""
echo "🔨 Building Swift CLI..."

cd PlotterCLI
swift build -c release

echo ""
echo "✅ Build complete!"
echo ""
echo "📦 Executable location:"
echo "   $(pwd)/.build/release/plotter"
echo ""
echo "💡 To install:"
echo "   sudo cp .build/release/plotter /usr/local/bin/"
echo ""
echo "💡 Or create an alias:"
echo "   alias plotter='$(pwd)/.build/release/plotter'"
