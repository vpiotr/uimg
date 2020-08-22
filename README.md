# Overview
Minimal, independent library for basic scientific diagram generation.
Portable, does not depend on OS or any third-party libraries.

# Features

Library provided two levels of abstraction:
* pixel canvas 
* draving primitives: lines, rectangles, circles, ellipses, b-splines, triangles, flood filling

Output: currently only PPM files (batch generation).

# Samples

1) Voronoi

Pixel map depicting Voronoi diagram.

See apps/voronoi.cpp

Sample output: ![Voronoi diagram](https://github.com/vpiotr/uimg/images/voronoi.ppm)

2) Mandelbrot

Pixel map image for selected window.

See apps/mandelbrot.cpp

Sample output: ![Mandelbrot](https://github.com/vpiotr/uimg/images/mandelbrot.ppm)

3) 3D Chart

Very basic 3D chart using colorful lines.

See apps/chart3d.cpp

Sample output: ![3d chart](https://github.com/vpiotr/uimg/images/chart3d.ppm)

4) Primitives

Demonstration of included primitive shape support.

See apps/draw_primitives.cpp

Sample output: ![Primitives](https://github.com/vpiotr/uimg/images/primitives.ppm) 

# See also
For more advanced plots/charts see:

* https://github.com/thclark/cpplot
* https://github.com/jkriege2/JKQtPlotter/

