#!/bin/bash

echo "=== 3D Chart Demo - Multi-Chart Support ==="
echo ""

# Single chart examples
echo "Generating single chart (default sinc function)..."
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 1 -out build/chart3d_single.ppm
echo "Created: build/chart3d_single.ppm"
echo ""

echo "Generating single chart with anti-aliasing..."
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 1 -aa -out build/chart3d_single_aa.ppm
echo "Created: build/chart3d_single_aa.ppm"
echo ""

# Dual chart examples
echo "Generating dual charts (sinc + gaussian)..."
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 2 -out build/chart3d_dual.ppm
echo "Created: build/chart3d_dual.ppm"
echo ""

echo "Generating dual charts with anti-aliasing..."
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 2 -aa -out build/chart3d_dual_aa.ppm
echo "Created: build/chart3d_dual_aa.ppm"
echo ""

# Triple chart example
echo "Generating triple charts (sinc + gaussian + ripple)..."
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 3 -out build/chart3d_triple.ppm
echo "Created: build/chart3d_triple.ppm"
echo ""

# Quad chart examples
echo "Generating quad charts (all functions)..."
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 4 -out build/chart3d_quad.ppm
echo "Created: build/chart3d_quad.ppm"
echo ""

echo "Generating quad charts with anti-aliasing..."
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 4 -aa -out build/chart3d_quad_aa.ppm
echo "Created: build/chart3d_quad_aa.ppm"
echo ""

echo "=== All multi-chart examples generated successfully! ==="
echo ""
echo "Usage examples:"
echo "  Single chart:  ./build/chart3d -charts 1 -out output.ppm"
echo "  Dual charts:   ./build/chart3d -charts 2 -out output.ppm"
echo "  Triple charts: ./build/chart3d -charts 3 -out output.ppm"
echo "  Quad charts:   ./build/chart3d -charts 4 -out output.ppm"
echo "  With AA:       ./build/chart3d -charts 4 -aa -out output.ppm"
echo ""
echo "Available functions: sinc, gaussian, ripple, saddle"
echo "Chart layouts are automatically determined based on number of charts."

