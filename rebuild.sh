#!/bin/bash

# rebuild.sh - Rebuilds everything from scratch using CMake
# Makes sure only "build" directory is used and CMake is used

# Check for --show-warnings flag
SHOW_WARNINGS=false
if [[ "$1" == "--show-warnings" ]]; then
    SHOW_WARNINGS=true
    echo "=== UIMG Project Rebuild Script (with warnings) ==="
else
    echo "=== UIMG Project Rebuild Script (warnings suppressed) ==="
    echo "Use './rebuild.sh --show-warnings' to see compiler warnings"
fi

echo "Rebuilding everything from scratch..."

# Get the script directory (project root)
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

echo "Project root: $PROJECT_ROOT"
echo "Build directory: $BUILD_DIR"

# Clean any existing CMake files from project root to avoid conflicts
echo "Cleaning any existing CMake files from project root..."
rm -f "$PROJECT_ROOT/CMakeCache.txt"
rm -rf "$PROJECT_ROOT/CMakeFiles"
rm -f "$PROJECT_ROOT/Makefile"
rm -f "$PROJECT_ROOT/cmake_install.cmake"

# Clean existing build directory
if [ -d "$BUILD_DIR" ]; then
    echo "Removing existing build directory..."
    rm -rf "$BUILD_DIR"
fi

# Create fresh build directory
echo "Creating fresh build directory..."
mkdir -p "$BUILD_DIR"

# Change to build directory
cd "$BUILD_DIR"

# Run CMake configuration
echo "Running CMake configuration..."
cmake "$PROJECT_ROOT"

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed!"
    exit 1
fi

# Build the project (we're already in build directory)
echo "Building the project..."
if [ "$SHOW_WARNINGS" = true ]; then
    make -j$(nproc)
    BUILD_RESULT=$?
else
    # Redirect stderr to /dev/null to suppress all warnings, but show progress
    make -j$(nproc) 2>/dev/null
    BUILD_RESULT=$?
    # If build failed, run again with stderr to show the actual error
    if [ $BUILD_RESULT -ne 0 ]; then
        echo "Build failed. Showing error details:"
        make -j$(nproc)
    fi
fi

if [ $BUILD_RESULT -ne 0 ]; then
    echo "ERROR: Build failed!"
    exit 1
fi

# Build tests separately
echo "Building tests..."
cd "$PROJECT_ROOT/tests"
mkdir -p build
cd build
cmake ..
if [ "$SHOW_WARNINGS" = true ]; then
    make -j$(nproc)
    TEST_BUILD_RESULT=$?
else
    # Redirect stderr to /dev/null to suppress all warnings, but show progress
    make -j$(nproc) 2>/dev/null
    TEST_BUILD_RESULT=$?
    # If build failed, run again with stderr to show the actual error
    if [ $TEST_BUILD_RESULT -ne 0 ]; then
        echo "Test build failed. Showing error details:"
        make -j$(nproc)
    fi
fi

if [ $TEST_BUILD_RESULT -ne 0 ]; then
    echo "ERROR: Tests build failed!"
    exit 1
fi

echo "=== Rebuild completed successfully! ==="
echo "Executables are available in:"
echo "  - Demos: $BUILD_DIR/"
echo "  - Tests: $BUILD_DIR/tests/"

# Verify build structure
echo ""
echo "=== Build Verification ==="
demo_count=$(find "$BUILD_DIR" -maxdepth 1 -type f -executable | wc -l)
test_count=$(find "$BUILD_DIR/tests" -maxdepth 1 -type f -executable 2>/dev/null | wc -l)
old_binaries=$(find "$PROJECT_ROOT" -maxdepth 1 -type f -executable -not -name "*.sh" | wc -l)

echo "Demo executables: $demo_count"
echo "Test executables: $test_count"

if [ $old_binaries -eq 0 ]; then
    echo "✓ No old binaries in root directory"
else
    echo "⚠ Found $old_binaries old binaries in root directory"
fi

if [ $demo_count -gt 0 ] && [ $test_count -gt 0 ] && [ $old_binaries -eq 0 ]; then
    echo "✓ Build structure is optimal!"
else
    echo "⚠ Build structure may need attention"
fi
