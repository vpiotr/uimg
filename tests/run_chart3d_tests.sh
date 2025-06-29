#!/bin/bash

# Chart3D Test Runner
# This script runs the multi-chart3d boundary tests

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

PROJECT_ROOT="/home/piotr/progs/oss_projects/uimg"
BUILD_DIR="$PROJECT_ROOT/build/tests"

echo -e "${BLUE}=== Multi-Chart3D Boundary Test Suite ===${NC}"
echo "Project root: $PROJECT_ROOT"
echo "Build directory: $BUILD_DIR"
echo

# Chart3D tests
CHART3D_TESTS=(
    "test_multi_chart3d_boundaries_simple"
)

# Run chart3d tests
PASSED=0
FAILED=0
FAILED_TESTS=()

echo -e "${YELLOW}Running Chart3D tests...${NC}"
echo

for test in "${CHART3D_TESTS[@]}"; do
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
echo -e "${BLUE}=== Chart3D Test Summary ===${NC}"
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
    echo -e "${RED}Some Chart3D tests failed!${NC}"
    exit 1
else
    echo
    echo -e "${GREEN}All Chart3D tests passed! 🎉${NC}"
    echo
    echo -e "${YELLOW}Test Details:${NC}"
    echo "• Boundary violation detection: ✓"
    echo "• Chart layout parsing: ✓"
    echo "• Pixel tracking functionality: ✓"
    echo "• Canvas bounds verification: ✓"
    echo "• Multi-chart region isolation: ✓"
    echo
    exit 0
fi
