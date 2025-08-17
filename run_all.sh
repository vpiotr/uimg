#!/bin/bash

# run_all.sh - Runs all demos with specific parameters to generate outputs
# This script runs all available demos with optimal parameters

echo "=== Running All Demos ==="

# Change to project root
cd /home/piotr/progs/oss_projects/uimg

# Ensure demos are built
echo "Building demos first..."
./build_all_demos.sh

echo "Running demos with specific parameters..."

# 2D Line Chart Demo
echo "Running 2D Line Chart Demo..."
./build/demos/2d_line_chart_demo -font fonts/courR12.bdf -charts 2 -thickness 1 -out output/2d_line_chart_aa.ppm -aa
./build/demos/2d_line_chart_demo -font fonts/courR12.bdf -charts 2 -thickness 1 -out output/2d_line_chart_noaa.ppm

# Chart3D Demo
echo "Running Chart3D Demo..."
./build/demos/chart3d_demo -out output/chart3d_single.ppm

# Multi Chart3D Demo
echo "Running Multi Chart3D Demo..."
./build/demos/multi_chart3d_demo -font fonts/courR12.bdf -size 1024x512 -charts 2 -layout auto -aa -dark -borders output/multi_chart3d_dark.ppm
./build/demos/multi_chart3d_demo -charts 4 -layout grid -out output/multi_chart3d_grid.ppm
./build/demos/multi_chart3d_demo -charts 3 -layout horizontal -out output/multi_chart3d_horizontal.ppm

# Text Demo
echo "Running Text Demo..."
./build/demos/text_demo -font fonts/courR12.bdf -out output/text_demo_output.ppm

# Filter Demo
echo "Running Filters Demo..."
./build/demos/filters_demo -out output/filters_demo_output.ppm

# Mandelbrot Demo
echo "Running Mandelbrot Demo..."
./build/demos/mandelbrot_demo -out output/mandelbrot.ppm

# Voronoi Demo
echo "Running Voronoi Demo..."
./build/demos/voronoi_demo -out output/voronoi.ppm

# Draw Primitives Demo
echo "Running Draw Primitives Demo..."
./build/demos/draw_primitives_demo -out output/primitives.ppm

# Basic Logging Demo
echo "Running Basic Logging Demo..."
./build/demos/basic_logging_demo

# Cast Demo
echo "Running Cast Demo..."
./build/demos/cast_demo

# Cast Validation Disabled Demo
echo "Running Cast Validation Disabled Demo..."
./build/demos/cast_validation_disabled_demo

# Observers Demo
echo "Running Observers Demo..."
./build/demos/observers_demo

echo "=== All demos completed! ==="
echo "Output files are available in the 'output/' directory"
