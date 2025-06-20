#!/bin/bash

# Script to restore original demo files after refactoring

echo "Restoring original demo files..."

# Restore base painter class
rm -f /home/piotr/progs/oss_projects/uimg/demos/include/samples/demo_painter_base.h

# Restore original demo files
for file in draw_primitives mandelbrot voronoi text_demo filter_demo chart3d; do
  # Remove our newly created files
  rm -f /home/piotr/progs/oss_projects/uimg/demos/include/samples/${file}/${file}_demo.h
  rm -f /home/piotr/progs/oss_projects/uimg/demos/include/chart3d/${file}_demo.h
  rm -f /home/piotr/progs/oss_projects/uimg/demos/include/samples/${file}/${file}_args.h
done

rm -f /home/piotr/progs/oss_projects/uimg/demos/include/chart3d/chart_z_fxy_3d_demo.h
rm -f /home/piotr/progs/oss_projects/uimg/demos/include/chart3d/multi_chart_3d_demo.h

echo "Restoring complete. Please run rebuild.sh to verify."
