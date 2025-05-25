# Overview
Minimal, independent library for basic scientific diagram generation.
Portable, does not depend on OS or any third-party libraries.

# Features

Library provided two levels of abstraction:
* pixel canvas 
* drawing primitives: lines, rectangles, circles, ellipses, b-splines, triangles, flood filling

Output: currently only PPM files (batch generation).

# Samples

1) 2D Line Chart

Line chart with customizable options including anti-aliasing for smoother line rendering.

See [apps/2d_line_chart_demo.cpp](apps/2d_line_chart_demo.cpp)

Usage:
```
./build/2d_line_chart_demo -font fonts/courR12.bdf -charts 2 -out output.ppm -aa
```

Sample output (anti-aliased): 

![2D Line Chart](images/2d_line_chart_aa.png?raw=true)

2) Voronoi

Pixel map depicting Voronoi diagram.

See [apps/voronoi.cpp](apps/voronoi.cpp)

Sample output: 

![Voronoi diagram](images/voronoi.png?raw=true)

3) Mandelbrot

Pixel map image for selected window.

See [apps/mandelbrot.cpp](apps/mandelbrot.cpp)

Sample output: 

![Mandelbrot](images/mandelbrot.png?raw=true)

4) 3D Chart

Customizable 3D surface charts with support for multiple functions and layouts. Features include:
- Optional anti-aliasing for smoother line rendering
- Multiple charts on a single image
- Customizable chart position, size, and appearance
- Several built-in function types (Sinc, Gaussian, Ripple, Saddle)

See [apps/chart3d.cpp](apps/chart3d.cpp)

Usage:
```
# Single chart with anti-aliasing
./build/chart3d -out output.ppm -aa

# Two charts with anti-aliasing
./build/chart3d -out output.ppm -aa -charts 2

# Four charts in grid layout
./build/chart3d -out output.ppm -aa -charts 4

# Four charts without titles
./build/chart3d -out output.ppm -aa -charts 4 -notitle

# Legacy mode (for backward compatibility)
./build/chart3d -out output.ppm -aa -legacy
```

Sample output (anti-aliased):
 
![3d chart](images/chart3d.png?raw=true)

5) Text Demo

Demonstration of BDF font rendering capabilities.

See [apps/text_demo.cpp](apps/text_demo.cpp)

Sample output:

![Text Demo](images/text_demo_output.png?raw=true)

6) Filter Demo

Demonstration of various pixel filters including transformations, transparency, zoom, and gradients.

See [apps/filter_demo.cpp](apps/filter_demo.cpp)

Sample output:

![Filter Demo](images/filter_demo_output.png?raw=true)

7) Primitives

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
* fonts: BDF font support
    * `uimg::BdfFont`: font representation class for BDF fonts
    * `uimg::BdfGlyph`: representation of individual glyphs with bitmap data
    * `uimg::FontUtils`: utilities for font loading and management
    * `uimg::TextPainterForBdfFont`: specialized painter for rendering BDF fonts
* text: Text rendering
    * `uimg::TextSource`: interface for text content sources
    * `uimg::TextPainter`: abstract interface for text rendering
    * Text positioning and alignment utilities
    * Multi-color text rendering support
* filters: Pixel filter transformations
    * `PixelFilter`: base class for all filters
    * Transformation filters: rotation, flipping, offset, zoom
    * Visual effect filters: transparency, alpha blending
    * Gradient filters: linear gradients

# Pixel filters

The library provides a rich set of pixel filter transformations through the `PixelFilter` class hierarchy. These filters can be chained together to create complex transformations and visual effects.

## Filter types

### Transformation filters
- **DiscreteRotationFilter**: Rotate pixels by discrete angles (-180°, -90°, 0°, 90°, 180°)
- **HorizontalFlipFilter**: Mirror pixels horizontally around a center point
- **VerticalFlipFilter**: Mirror pixels vertically around a center point
- **OffsetFilter**: Move pixels by a specified offset
- **ClipFilter**: Limit drawing operations to a specified rectangular region

### Visual effect filters
- **TransparencyFilter**: Make specific colors transparent
- **MaskDiffFilter**: Apply pixels only where mask differs from specified color
- **MaskEqFilter**: Apply pixels only where mask equals specified color
- **AlphaFilter**: Blend pixels with existing image using alpha transparency

### Zoom and scaling filters
- **PixelSpreadFilter**: Scale pixels with defined center point and zoom factor
- **PixelZoomFilter**: Scale pixels with expansion, filling gaps between pixels

### Gradient filters
- **GradientFilter2C**: Apply a linear gradient between two colors

## Using filters

Filters can be easily combined by chaining. Each filter is initialized with the target painter and its specific parameters:

```cpp
// Create a base painter
PixelPainterForRgbImage basePainter(image);

// Apply a horizontal flip filter
HorizontalFlipFilter flipFilter(basePainter, Point(width / 2, 0));

// Add transparency
TransparencyFilter transFilter(flipFilter, RgbColor(255, 0, 255));

// Use the filter chain
transFilter.putPixel(x, y, color);
```

The filter demo application showcases all available filters and their combinations.

# Text rendering, font support

The library provides a comprehensive text rendering system using BDF fonts. Text rendering is implemented through the `uimg::TextSource` interface and the `uimg::TextPainter` abstract class. The concrete implementation `uimg::TextPainterForBdfFont` handles rendering for BDF fonts.

## Features
- Loading and rendering BDF fonts
- Multi-color text rendering
- Precise text positioning
- Support for text alignment
- Basic text formatting

## Text Demo Application

The `text_demo` application demonstrates the text rendering capabilities:
- Renders text with different colors
- Shows various formatting options
- Displays proper text positioning and alignment
- Renders sample informational text

To run the text demo:

```
./text_demo -font <path/to/font.bdf> -out <output_file.ppm>
```

Example:
```
./text_demo -font ../fonts/courR12.bdf -out text_output.ppm
```

## BDF Font Format
The library uses the BDF (Bitmap Distribution Format) font format:
* Format description:
  * https://en.wikipedia.org/wiki/Glyph_Bitmap_Distribution_Format
  * https://partners.adobe.com/public/developer/en/font/5005.BDF_Spec.pdf
* X11 font description
  * http://www.linuxfromscratch.org/blfs/view/svn/x/x7font.html

Fonts can be downloaded from:
* http://ftp.x.org/pub/individual/font/
  * example: font-adobe-75dpi-1.0.0.tar.gz / courR12.bdf

Sample fonts are included in the `fonts` directory.

# See also

For more advanced plots/charts see:
* https://github.com/thclark/cpplot
* https://github.com/jkriege2/JKQtPlotter/

