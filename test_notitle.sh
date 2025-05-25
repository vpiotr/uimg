#!/bin/bash
# This script tests the new -notitle feature by generating two chart images for comparison

# Create a chart with titles (default)
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 4 -layout grid -aa -out build/chart3d_with_titles.ppm

# Create a chart without titles
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 4 -layout grid -aa -notitle -out build/chart3d_no_titles.ppm

echo "Test completed. Compare the output files:"
echo "build/chart3d_with_titles.ppm - Should show titles"
echo "build/chart3d_no_titles.ppm - Should NOT show titles"
