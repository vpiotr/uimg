#!/bin/bash

# Build all demos script
# This script builds all demos individually using out-of-source builds
# All build artifacts go to build/demos/<demo-name>/
# Executables are moved to build/demos/

echo "Building all demos..."

# Create build directory
mkdir -p build/demos

# List of demo directories and their executable names
declare -A DEMOS=(
    ["2d_line_chart"]="2d_line_chart_demo"
    ["basic_logging"]="basic_logging_demo"
    ["cast"]="cast_demo"
    ["cast_validation_disabled"]="cast_validation_disabled_demo"
    ["chart3d"]="chart3d_demo"
    ["draw_primitives"]="draw_primitives_demo"
    ["filters"]="filters_demo"
    ["mandelbrot"]="mandelbrot_demo"
    ["multi_chart3d"]="multi_chart3d_demo"
    ["observers"]="observers_demo"
    ["text"]="text_demo"
    ["voronoi"]="voronoi_demo"
)

# Build each demo individually using out-of-source builds
for demo in "${!DEMOS[@]}"; do
    echo "Building $demo..."
    executable="${DEMOS[$demo]}"
    
    # Create build directory for this demo
    mkdir -p "build/demos/$demo"
    cd "build/demos/$demo"
    
    # Run cmake pointing to the source directory
    cmake "../../../demos/$demo" > /dev/null 2>&1
    make > /dev/null 2>&1
    
    # Move the executable to the parent directory if it exists
    if [ -f "$executable" ]; then
        mv "$executable" "../$executable"
    else
        # Check if executable has different name (without _demo suffix)
        base_name=$(echo "$executable" | sed 's/_demo$//')
        if [ -f "$base_name" ]; then
            mv "$base_name" "../$executable"
        fi
    fi
    
    cd ../../..
done

echo "All demos built successfully!"
echo "Executables available in build/demos/"
echo ""
echo "Available demos:"
ls -la build/demos/ 2>/dev/null || echo "Build directory not found. Run the build first."
