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

See [apps/voronoi.cpp](apps/voronoi.cpp)

Sample output: 

![Voronoi diagram](images/voronoi.png?raw=true)

2) Mandelbrot

Pixel map image for selected window.

See [apps/mandelbrot.cpp](apps/mandelbrot.cpp)

Sample output: 

![Mandelbrot](images/mandelbrot.png?raw=true)

3) 3D Chart

Very basic 3D chart using colorful lines.

See [apps/chart3d.cpp](apps/chart3d.cpp)

Sample output:
 
![3d chart](images/chart3d.png?raw=true)

4) Primitives

Demonstration of included primitive shape support.

See [apps/draw_primitives.cpp](apps/draw_primitives.cpp)

Sample output: 

![Primitives](images/primitives.png?raw=true) 

# Building from console

1) Create new build folder

        mkdir _build
        cd _build

2) Prepare makefile

        cmake .. 

3) make sample applications

        make

4) to run just built example - voronoi - just enter:

        ./voronoi

# API
Currently there is no single include file which can be used to use the library.
So you need to include individual header files in order to use specific features.

Source directories:

* base: data structures 
* utils: internal utility functions for various purposes 
* pixels: pixel-level API 
* images: API for images 
    * PixelImage: image abstraction 
    * RgbImage: in-memory image container 
    * PPM image writer & loader
* painters: API for drawing graphic primitives (lines, circles, etc.)     
 
# See also
For more advanced plots/charts see:

* https://github.com/thclark/cpplot
* https://github.com/jkriege2/JKQtPlotter/

