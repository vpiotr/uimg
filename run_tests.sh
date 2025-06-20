#!/bin/bash

# run_tests.sh - Runs all tests via CMake
echo "=== UIMG Project Test Runner ==="
echo "Running all tests..."

# Get the script directory (project root)
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
TESTS_DIR="$PROJECT_ROOT/tests"

echo "Project root: $PROJECT_ROOT"

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "ERROR: Build directory not found. Please run rebuild.sh first."
    exit 1
fi

# Run main project tests using CTest
echo "Running main project tests with CTest..."
cd "$BUILD_DIR"
ctest --verbose

MAIN_TEST_RESULT=$?

# Run additional tests (dlog, unsigned_cast, etc.)
echo ""
echo "Running additional tests..."
cd "$TESTS_DIR"

# Clean any existing CMake files from tests directory
rm -f CMakeCache.txt
rm -rf CMakeFiles
rm -f Makefile
rm -f cmake_install.cmake

# Check if tests build directory exists
if [ ! -d "build" ]; then
    echo "Creating tests build directory..."
    mkdir -p build
fi

cd build

echo "Building tests..."
cmake "$TESTS_DIR"
if [ $? -ne 0 ]; then
    echo "ERROR: Tests CMake configuration failed!"
    exit 1
fi

make -j$(nproc)
if [ $? -ne 0 ]; then
    echo "ERROR: Tests build failed!"
    exit 1
fi

# Run the test script (skip the old one and run tests manually)
echo "Running individual test executables manually..."

# Run individual test executables from the main build/tests directory
ADDITIONAL_TEST_RESULT=0
TESTS_OUTPUT_DIR="$PROJECT_ROOT/build/tests"

if [ -d "$TESTS_OUTPUT_DIR" ]; then
    cd "$TESTS_OUTPUT_DIR"
    echo "Running tests from $TESTS_OUTPUT_DIR..."
    
    for test_exec in test_*; do
        if [ -x "$test_exec" ]; then
            echo "Running $test_exec..."
            ./"$test_exec"
            if [ $? -ne 0 ]; then
                ADDITIONAL_TEST_RESULT=1
            fi
        fi
    done
else
    echo "No tests output directory found at $TESTS_OUTPUT_DIR"
    ADDITIONAL_TEST_RESULT=1
fi

# Summary
echo ""
echo "=== Test Results Summary ==="
if [ $MAIN_TEST_RESULT -eq 0 ]; then
    echo "✓ Main project tests: PASSED"
else
    echo "✗ Main project tests: FAILED"
fi

if [ $ADDITIONAL_TEST_RESULT -eq 0 ]; then
    echo "✓ Additional tests: PASSED"
else
    echo "✗ Additional tests: FAILED"
fi

# Exit with error if any tests failed
if [ $MAIN_TEST_RESULT -ne 0 ] || [ $ADDITIONAL_TEST_RESULT -ne 0 ]; then
    echo ""
    echo "Some tests failed!"
    exit 1
else
    echo ""
    echo "All tests passed!"
    exit 0
fi
