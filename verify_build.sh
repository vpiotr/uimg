#!/bin/bash

# verify_build.sh - Verify that all binaries are in the correct build directories

echo "=== UIMG Build Verification ==="
echo "Checking build directory structure..."

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
TESTS_DIR="$BUILD_DIR/tests"

# Check main build directory
echo ""
echo "Demo binaries in $BUILD_DIR:"
if [ -d "$BUILD_DIR" ]; then
    demo_count=$(find "$BUILD_DIR" -maxdepth 1 -type f -executable | wc -l)
    echo "  Found $demo_count demo executables:"
    ls -1 "$BUILD_DIR" | grep -v "\\.\\|CMake\\|Makefile" | sort
else
    echo "  ERROR: Build directory not found!"
    exit 1
fi

# Check tests directory
echo ""
echo "Test binaries in $TESTS_DIR:"
if [ -d "$TESTS_DIR" ]; then
    test_count=$(find "$TESTS_DIR" -maxdepth 1 -type f -executable | wc -l)
    echo "  Found $test_count test executables:"
    ls -1 "$TESTS_DIR" 2>/dev/null | sort
else
    echo "  ERROR: Tests directory not found!"
    exit 1
fi

# Check for old binaries in root
echo ""
echo "Checking for old binaries in project root:"
old_binaries=$(find "$PROJECT_ROOT" -maxdepth 1 -type f -executable -not -name "*.sh" | wc -l)
if [ $old_binaries -eq 0 ]; then
    echo "  ✓ No old binaries found in root directory"
else
    echo "  ✗ Found $old_binaries old binaries in root directory:"
    find "$PROJECT_ROOT" -maxdepth 1 -type f -executable -not -name "*.sh"
fi

echo ""
echo "=== Build Verification Complete ==="
if [ $old_binaries -eq 0 ] && [ $demo_count -gt 0 ] && [ $test_count -gt 0 ]; then
    echo "✓ Build structure is correct!"
    exit 0
else
    echo "✗ Build structure has issues!"
    exit 1
fi
