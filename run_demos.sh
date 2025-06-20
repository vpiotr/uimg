#!/bin/bash

# run_demos.sh - Runs all demos
echo "=== UIMG Project Demo Runner ==="
echo "Running all demos..."

# Get the script directory (project root)
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"

echo "Project root: $PROJECT_ROOT"

# Check if build directory exists
if [ ! -d "$BUILD_DIR" ]; then
    echo "ERROR: Build directory not found. Please run rebuild.sh first."
    exit 1
fi

# Change to build directory
cd "$BUILD_DIR"

# List of demo executables (based on CMakeLists.txt)
DEMOS=(
    "voronoi"
    "mandelbrot" 
    "chart3d"
    "draw_primitives"
    "filter_demo"
    "2d_line_chart_demo"
    "text_demo"
    "observers_demo"
    "basic_logging_demo"
    "cast_demo"
    "cast_validation_disabled_demo"
)

echo "Available demos: ${DEMOS[*]}"
echo ""

# Function to run a demo with error handling
run_demo() {
    local demo_name="$1"
    
    if [ -x "./$demo_name" ]; then
        echo "--- Running $demo_name ---"
        
        # Handle demos that require specific arguments
        case "$demo_name" in
            "2d_line_chart_demo")
                "./$demo_name" -font ../fonts/courR12.bdf
                ;;
            "text_demo")
                "./$demo_name" -font ../fonts/courR12.bdf
                ;;
            "chart3d")
                "./$demo_name" "chart3d.ppm"
                ;;
            "draw_primitives")
                "./$demo_name" "primitives.ppm"
                ;;
            "voronoi")
                "./$demo_name" "voronoi.ppm"
                ;;
            "mandelbrot")
                "./$demo_name" "mandelbrot.ppm"
                ;;
            *)
                "./$demo_name"
                ;;
        esac
        
        local result=$?
        if [ $result -eq 0 ]; then
            echo "✓ $demo_name completed successfully"
        else
            echo "✗ $demo_name failed with exit code $result"
        fi
        echo ""
        return $result
    else
        echo "✗ $demo_name executable not found or not executable"
        echo ""
        return 1
    fi
}

# Run all demos
TOTAL_DEMOS=0
SUCCESSFUL_DEMOS=0

for demo in "${DEMOS[@]}"; do
    TOTAL_DEMOS=$((TOTAL_DEMOS + 1))
    
    if run_demo "$demo"; then
        SUCCESSFUL_DEMOS=$((SUCCESSFUL_DEMOS + 1))
    fi
done

# Summary
echo "=== Demo Results Summary ==="
echo "Successful demos: $SUCCESSFUL_DEMOS/$TOTAL_DEMOS"

if [ $SUCCESSFUL_DEMOS -eq $TOTAL_DEMOS ]; then
    echo "✓ All demos completed successfully!"
    exit 0
else
    echo "✗ Some demos failed to run"
    exit 1
fi
