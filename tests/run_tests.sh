#!/bin/bash

# Test runner script for ulog library
# This script compiles and runs all ulog tests

# Note: Removed 'set -e' to handle test failures gracefully

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

PROJECT_ROOT="/home/piotr/progs/oss_projects/uimg"
TESTS_DIR="$PROJECT_ROOT/tests"
BUILD_DIR="$PROJECT_ROOT/build/tests"
INCLUDE_DIR="$PROJECT_ROOT/include"

echo -e "${BLUE}=== ulog Library Test Suite ===${NC}"
echo "Project root: $PROJECT_ROOT"
echo "Tests directory: $TESTS_DIR"
echo

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Ensure output directory exists
mkdir -p "$TESTS_DIR/output"

# List of test files
TESTS=(
    "test_ulog_console"
    "test_ulog_inmemory"
    "test_ulog_file"
    "test_ulog_multi"
    "test_ulog_utils"
)

# Compilation flags
CXXFLAGS="-std=c++11 -Wall -Wextra -I$INCLUDE_DIR"

echo -e "${YELLOW}Compiling tests...${NC}"

# Compile each test
for test in "${TESTS[@]}"; do
    echo "Compiling $test..."
    if g++ $CXXFLAGS -o "$BUILD_DIR/$test" "$TESTS_DIR/ulog/$test.cpp"; then
        echo -e "${GREEN}✓ $test compiled successfully${NC}"
    else
        echo -e "${RED}✗ Failed to compile $test${NC}"
        exit 1
    fi
done

echo
echo -e "${YELLOW}Running tests...${NC}"
echo

# Run each test
PASSED=0
FAILED=0
FAILED_TESTS=()

for test in "${TESTS[@]}"; do
    echo -e "${BLUE}Running $test...${NC}"
    echo "----------------------------------------"
    
    if "$BUILD_DIR/$test"; then
        echo -e "${GREEN}✓ $test PASSED${NC}"
        ((PASSED++))
    else
        echo -e "${RED}✗ $test FAILED${NC}"
        ((FAILED++))
        FAILED_TESTS+=("$test")
    fi
    
    echo "----------------------------------------"
    echo
done

# Summary
echo -e "${BLUE}=== Test Summary ===${NC}"
echo "Total tests: $((PASSED + FAILED))"
echo -e "${GREEN}Passed: $PASSED${NC}"
echo -e "${RED}Failed: $FAILED${NC}"

if [ $FAILED -gt 0 ]; then
    echo
    echo -e "${RED}Failed tests:${NC}"
    for failed_test in "${FAILED_TESTS[@]}"; do
        echo "  - $failed_test"
    done
    echo
    echo -e "${RED}Some tests failed!${NC}"
    exit 1
else
    echo
    echo -e "${GREEN}All tests passed! 🎉${NC}"
    
    # Clean up test output files
    echo "Cleaning up test output files..."
    rm -f "$TESTS_DIR/output"/*.log
    echo "Test output files cleaned."
    
    exit 0
fi
