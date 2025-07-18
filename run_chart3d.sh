#!/bin/bash

echo "=== 3D Chart Demo - Multi-Chart Support ==="
echo ""

# Single chart examples
echo "Generating single chart (default sinc function)..."
cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 1 -out build/chart3d_single.ppm
echo "Created: build/chart3d_single.ppm"
echo ""

echo "Generating single chart with anti-aliasing..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 1 -aa -out build/chart3d_single_aa.ppm
echo "Created: build/chart3d_single_aa.ppm"
echo ""

echo "Generating single chart with borders..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 1 -borders -out build/chart3d_single_borders.ppm
echo "Created: build/chart3d_single_borders.ppm"
echo ""

echo "Generating single chart with borders and anti-aliasing..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 1 -borders -aa -out build/chart3d_single_borders_aa.ppm
echo "Created: build/chart3d_single_borders_aa.ppm"
echo ""

# Dual chart examples
echo "Generating dual charts (sinc + gaussian)..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 2 -out build/chart3d_dual.ppm
echo "Created: build/chart3d_dual.ppm"
echo ""

echo "Generating dual charts with anti-aliasing..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 2 -aa -out build/chart3d_dual_aa.ppm
echo "Created: build/chart3d_dual_aa.ppm"
echo ""

echo "Generating dual charts with borders..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 2 -borders -out build/chart3d_dual_borders.ppm
echo "Created: build/chart3d_dual_borders.ppm"
echo ""

echo "Generating dual charts with borders and anti-aliasing..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 2 -borders -aa -out build/chart3d_dual_borders_aa.ppm
echo "Created: build/chart3d_dual_borders_aa.ppm"
echo ""

# Triple chart example
echo "Generating triple charts (sinc + gaussian + ripple)..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 3 -out build/chart3d_triple.ppm
echo "Created: build/chart3d_triple.ppm"
echo ""

echo "Generating triple charts with borders..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 3 -borders -out build/chart3d_triple_borders.ppm
echo "Created: build/chart3d_triple_borders.ppm"
echo ""

echo "Generating triple charts with borders and anti-aliasing..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 3 -borders -aa -out build/chart3d_triple_borders_aa.ppm
echo "Created: build/chart3d_triple_borders_aa.ppm"
echo ""

# Quad chart examples
echo "Generating quad charts (all functions)..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 4 -out build/chart3d_quad.ppm
echo "Created: build/chart3d_quad.ppm"
echo ""

echo "Generating quad charts with anti-aliasing..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 4 -aa -out build/chart3d_quad_aa.ppm
echo "Created: build/chart3d_quad_aa.ppm"
echo ""

echo "Generating quad charts with borders..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 4 -borders -out build/chart3d_quad_borders.ppm
echo "Created: build/chart3d_quad_borders.ppm"
echo ""

echo "Generating quad charts with borders and anti-aliasing..."
rem cd /home/piotr/progs/oss_projects/uimg && ./build/chart3d -charts 4 -borders -aa -out build/chart3d_quad_borders_aa.ppm
echo "Created: build/chart3d_quad_borders_aa.ppm"
echo ""

echo "=== All multi-chart examples generated successfully! ==="
echo ""

echo "=== Testing new multi_chart3d demo ==="
echo ""

echo "Generating single chart with multi_chart3d demo..."
cd /home/piotr/progs/oss_projects/uimg && ./build/multi_chart3d -charts 1 build/multi_chart3d_single.ppm
echo "Created: build/multi_chart3d_single.ppm"
echo ""

echo "Generating dual charts with multi_chart3d demo..."
cd /home/piotr/progs/oss_projects/uimg && ./build/multi_chart3d -charts 2 -layout horizontal build/multi_chart3d_dual_horizontal.ppm
echo "Created: build/multi_chart3d_dual_horizontal.ppm"
echo ""

echo "Generating triple charts with anti-aliasing..."
cd /home/piotr/progs/oss_projects/uimg && ./build/multi_chart3d -charts 3 -aa build/multi_chart3d_triple_aa.ppm
echo "Created: build/multi_chart3d_triple_aa.ppm"
echo ""

echo "Generating quad charts with borders and anti-aliasing..."
cd /home/piotr/progs/oss_projects/uimg && ./build/multi_chart3d -charts 4 -borders -aa -layout 2x2 build/multi_chart3d_quad_2x2.ppm
echo "Created: build/multi_chart3d_quad_2x2.ppm"
echo ""
echo "Usage examples:"
echo "  Single chart:  ./build/chart3d -charts 1 -out output.ppm"
echo "  Dual charts:   ./build/chart3d -charts 2 -out output.ppm"
echo "  Triple charts: ./build/chart3d -charts 3 -out output.ppm"
echo "  Quad charts:   ./build/chart3d -charts 4 -out output.ppm"
echo "  With AA:       ./build/chart3d -charts 4 -aa -out output.ppm"
echo "  With borders:  ./build/chart3d -charts 2 -borders -out output.ppm"
echo "  Combined:      ./build/chart3d -charts 4 -borders -aa -out output.ppm"
echo ""
echo "Available functions: sinc, gaussian, ripple, saddle"
echo "Chart layouts are automatically determined based on number of charts."

