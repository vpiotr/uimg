#!/bin/bash

# clean.sh - Clean all build artifacts and old binaries

echo "=== UIMG Project Cleaner ==="
echo "Removing all build artifacts and old binaries..."

# Get the script directory (project root)
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Remove build directories
echo "Removing build directories..."
rm -rf "$PROJECT_ROOT/build"
rm -rf "$PROJECT_ROOT/tests/build"
rm -rf "$PROJECT_ROOT/cmake-build-debug"

# Remove CMake artifacts from root
echo "Removing CMake artifacts from project root..."
rm -f "$PROJECT_ROOT/CMakeCache.txt"
rm -rf "$PROJECT_ROOT/CMakeFiles"
rm -f "$PROJECT_ROOT/Makefile"
rm -f "$PROJECT_ROOT/cmake_install.cmake"
rm -f "$PROJECT_ROOT/CTestTestfile.cmake"

# Remove CMake artifacts from tests directory
echo "Removing CMake artifacts from tests directory..."
cd "$PROJECT_ROOT/tests"
rm -f CMakeCache.txt
rm -rf CMakeFiles
rm -f Makefile
rm -f cmake_install.cmake

# Remove any old binaries from root directory
echo "Removing old binaries from project root..."
old_binaries=$(find "$PROJECT_ROOT" -maxdepth 1 -type f -executable -not -name "*.sh" | wc -l)
if [ $old_binaries -gt 0 ]; then
    echo "Found $old_binaries old binaries:"
    find "$PROJECT_ROOT" -maxdepth 1 -type f -executable -not -name "*.sh" -exec basename {} \;
    find "$PROJECT_ROOT" -maxdepth 1 -type f -executable -not -name "*.sh" -delete
    echo "Removed $old_binaries old binaries"
else
    echo "No old binaries found in project root"
fi

# Remove temporary files
echo "Removing temporary files..."
find "$PROJECT_ROOT" -name "*.tmp" -delete 2>/dev/null || true
find "$PROJECT_ROOT" -name "*.log" -delete 2>/dev/null || true
find "$PROJECT_ROOT" -name "*~" -delete 2>/dev/null || true

echo ""
echo "=== Cleanup completed! ==="
echo "Project is now clean. Run './rebuild.sh' to build everything."
