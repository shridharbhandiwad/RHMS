#!/bin/bash
# Build script for Radar Health Monitoring System

set -e

# Configuration
BUILD_DIR="build"
BUILD_TYPE="Release"
CMAKE_GENERATOR=""

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --clean)
            rm -rf "$BUILD_DIR"
            echo "Cleaned build directory"
            shift
            ;;
        *)
            echo "Unknown option: $1"
            echo "Usage: $0 [--debug] [--clean]"
            exit 1
            ;;
    esac
done

# Detect platform
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Building for Linux..."
    CMAKE_GENERATOR="-G Ninja"
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "Building for macOS..."
    CMAKE_GENERATOR="-G Ninja"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
    echo "Building for Windows..."
    CMAKE_GENERATOR=""
fi

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Run CMake
echo "Configuring with CMake..."
cmake .. \
    $CMAKE_GENERATOR \
    -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

# Build
echo "Building..."
cmake --build . --config $BUILD_TYPE -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

echo ""
echo "Build completed successfully!"
echo "Build type: $BUILD_TYPE"
echo "Executable: $BUILD_DIR/RadarHealthMonitor"
