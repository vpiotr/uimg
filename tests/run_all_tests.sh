#!/bin/bash

# Universal Test Runner for uimg project
# This script runs all available tests

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

PROJECT_ROOT="/home/piotr/progs/oss_projects/uimg"
TESTS_DIR="$PROJECT_ROOT/tests"
BUILD_DIR="$PROJECT_ROOT/build/tests"

echo -e "${BLUE}=== uimg Project Test Suite ===${NC}"
echo "Project root: $PROJECT_ROOT"
echo "Tests directory: $TESTS_DIR"
echo "Build directory: $BUILD_DIR"
echo

# Ensure build directory exists
mkdir -p "$BUILD_DIR"

# Build tests using CMake
echo -e "${YELLOW}Building tests...${NC}"
cd "$TESTS_DIR"
if cmake . && make -j4; then
    echo -e "${GREEN}✓ All tests built successfully${NC}"
else
    echo -e "${RED}✗ Failed to build tests${NC}"
    exit 1
fi

echo

# Available test executables
ALL_TESTS=(
    "test_dlog_basic"
    "test_dlog_features"
    "test_unsigned_cast_basic"
    "test_unsigned_cast_failures"
    "test_multi_chart3d_boundaries_simple"
)

# Run all tests
PASSED=0
FAILED=0
FAILED_TESTS=()

echo -e "${YELLOW}Running all tests...${NC}"
echo

for test in "${ALL_TESTS[@]}"; do
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
echo -e "${BLUE}=== Final Test Summary ===${NC}"
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
    echo
    echo -e "${YELLOW}Test Coverage:${NC}"
    echo "• Logging functionality (dlog): ✓"
    echo "• Type casting utilities: ✓"
    echo "• Multi-chart3D boundary validation: ✓"
    echo "• Pixel tracking and analysis: ✓"
    echo
    exit 0
fi
