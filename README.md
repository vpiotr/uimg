# uimg - Micro Image Generation Library

A minimal, independent C++ library for basic scientific diagram generation and image processing. The library is designed to be portable and self-contained, with no dependencies on external libraries or operating system-specific APIs.

## Key Features

- **Zero dependencies**: No external libraries required
- **Cross-platform**: Works on any system with a C++ compiler
- **Scientific visualization**: Optimized for charts, diagrams, and data visualization
- **Pixel-level control**: Direct pixel manipulation API
- **Rich drawing primitives**: Lines, circles, rectangles, ellipses, B-splines, triangles, flood fill
- **Text rendering**: BDF font support with multi-color text
- **Image filters**: Comprehensive set of transformation and visual effect filters
- **Output formats**: PPM image format support

## Quick Start

To build the project from scratch:

```bash
./rebuild.sh    # Build everything (demos and tests)
./run_demos.sh  # Run all demo applications
./run_tests.sh  # Run all tests
./clean.sh      # Clean all build artifacts
```

All binaries are placed in the `build/` directory:
- Demo executables: `build/`
- Test executables: `build/tests/`

## Project Structure

```
uimg/
├── include/uimg/          # Public API headers
│   ├── base/             # Core data structures
│   ├── charts/           # Chart generation utilities
│   ├── filters/          # Pixel filter transformations
│   ├── fonts/            # BDF font support
│   ├── images/           # Image handling (PPM I/O, RGB)
│   ├── painters/         # Drawing primitives API
│   ├── pixels/           # Pixel-level API
│   ├── text/             # Text rendering
│   └── utils/            # Utility functions
├── demos/                # Example applications
├── tests/                # Unit tests
├── fonts/                # Sample BDF fonts
└── images/               # Sample outputs
```

# Demo Applications

The library includes several demonstration applications showcasing different capabilities:

## 1. 2D Line Chart Demo

Line chart with customizable options including anti-aliasing for smoother line rendering.

**Source:** [demos/2d_line_chart_demo.cpp](demos/2d_line_chart_demo.cpp)

**Usage:**
```bash
./build/2d_line_chart_demo -font fonts/courR12.bdf -charts 2 -out output.ppm -aa
```

**Sample output (anti-aliased):**

![2D Line Chart](images/2d_line_chart_aa.png?raw=true)

## 2. 3D Single Chart Demo

Basic 3D surface chart rendering for individual mathematical functions.

**Source:** [demos/chart3d.cpp](demos/chart3d.cpp)

**Usage:**
```bash
# Single chart with anti-aliasing
./build/chart3d chart3d.ppm
```

**Sample output:**

![3D single chart](images/chart3d.png?raw=true)

## 3. 3D Multi-Chart Demo

Advanced 3D surface chart demo with support for multiple functions, layouts, and comprehensive customization. Features include:
- Support for 1-4 charts displayed simultaneously on a single image
- Configurable image size and font paths
- Optional anti-aliasing for smoother line rendering
- Automatic layout management with special 3-chart layout (bottom chart spans full width)
- Mathematical formula titles for each chart
- Dark/light mode themes with borders
- Several built-in function types (Ripple, Mexican Hat, Gaussian, Peaks, etc.)

**Source:** [demos/multi_chart3d.cpp](demos/multi_chart3d.cpp)

**Usage:**
```bash
# Multi-chart demo with all features (as used in run_demos.sh from build directory)
./build/multi_chart3d -font fonts/courR12.bdf -size 1024X512 -charts 2 -layout auto -aa -dark -borders multi_chart3d.ppm

# Customizable examples
./build/multi_chart3d -charts 3 -size 1200x900 output.ppm
./build/multi_chart3d -charts 4 -layout 2x2 -borders output.ppm
```

**Sample output (dual charts with dark theme and borders):**

![3D dual chart](images/multi_chart3d_dark_2.png?raw=true)

## 4. Voronoi Diagram

Pixel map depicting Voronoi diagram generation.

**Source:** [demos/voronoi.cpp](demos/voronoi.cpp)

**Sample output:**

![Voronoi diagram](images/voronoi.png?raw=true)

## 5. Mandelbrot Set

Pixel map image for selected window of the Mandelbrot set.

**Source:** [demos/mandelbrot.cpp](demos/mandelbrot.cpp)

**Sample output:**

![Mandelbrot](images/mandelbrot.png?raw=true)

## 6. Drawing Primitives Demo

Demonstration of included primitive shape support including lines, circles, rectangles, ellipses, and other geometric shapes.

**Source:** [demos/draw_primitives.cpp](demos/draw_primitives.cpp)

**Sample output:**

![Primitives](images/primitives.png?raw=true)

## 7. Text Rendering Demo

Demonstration of BDF font rendering capabilities with multi-color text support.

**Source:** [demos/text_demo.cpp](demos/text_demo.cpp)

**Usage:**
```bash
./build/text_demo -font fonts/courR12.bdf -out text_output.ppm
```

**Sample output:**

![Text Demo](images/text_demo_output.png?raw=true)

## 8. Filter Demo

Demonstration of various pixel filters including transformations, transparency, zoom, and gradients.

**Source:** [demos/filter_demo.cpp](demos/filter_demo.cpp)

**Sample output:**

![Filter Demo](images/filter_demo_output.png?raw=true)

## 9. Logging Demo

Basic demonstration of the integrated logging system.

**Source:** [demos/basic_logging_demo.cpp](demos/basic_logging_demo.cpp)

## 10. Type Casting Demos

Demonstrations of safe type casting utilities:
- **Cast Demo**: [demos/cast_demo.cpp](demos/cast_demo.cpp) - Basic casting operations
- **Cast Validation Disabled**: [demos/cast_validation_disabled_demo.cpp](demos/cast_validation_disabled_demo.cpp) - Performance-oriented casting

## 11. Observer Pattern Demo

Demonstration of the observer pattern implementation for event handling.

**Source:** [demos/observers_demo.cpp](demos/observers_demo.cpp) 

# Manual Build Process

If you prefer to build manually or need more control over the build process:

## 1. Create build directory

```bash
mkdir _build
cd _build
```

## 2. Configure with CMake

```bash
cmake ..
```

## 3. Build the project

```bash
make
```

## 4. Run a demo

```bash
./voronoi
```

# API Reference

The library provides a layered API architecture. Include the specific header files you need for your use case.

## Core Modules

### Base (`include/uimg/base/`)
- Core data structures and fundamental types
- Point, Size, Rectangle classes
- Color representations (RGB, etc.)

### Pixels (`include/uimg/pixels/`)
- Low-level pixel manipulation API
- Direct pixel access and modification
- Pixel-level drawing operations

### Images (`include/uimg/images/`)
- **`PixelImage`**: Abstract image interface
- **`RgbImage`**: In-memory RGB image container
- **`PpmImageWriter`**: PPM format output
- **`PpmImageLoader`**: PPM format input

### Painters (`include/uimg/painters/`)
- High-level drawing API for graphic primitives
- Lines, circles, rectangles, ellipses
- B-splines, triangles, flood fill
- Anti-aliasing support

### Charts (`include/uimg/charts/`)
- Specialized chart generation utilities
- 2D line charts
- 3D surface plots
- Multi-chart layouts

### Fonts (`include/uimg/fonts/`)
- **`BdfFont`**: BDF font representation
- **`BdfGlyph`**: Individual glyph bitmap data
- **`FontUtils`**: Font loading and management utilities
- **`TextPainterForBdfFont`**: BDF font rendering

### Text (`include/uimg/text/`)
- **`TextSource`**: Interface for text content sources
- **`TextPainter`**: Abstract text rendering interface
- Text positioning and alignment utilities
- Multi-color text rendering support

### Filters (`include/uimg/filters/`)
- **`PixelFilter`**: Base class for all pixel transformations
- Transformation filters (rotation, flipping, offset, zoom)
- Visual effect filters (transparency, alpha blending)
- Gradient filters (linear gradients)

### Utilities (`include/uimg/utils/`)
- Internal utility functions
- Type casting utilities
- Mathematical helpers
- Observer pattern implementation

# Advanced Features

## Pixel Filters

The library provides a rich set of pixel filter transformations through the `PixelFilter` class hierarchy. These filters can be chained together to create complex transformations and visual effects.

### Filter Types

#### Transformation Filters
- **`DiscreteRotationFilter`**: Rotate pixels by discrete angles (-180°, -90°, 0°, 90°, 180°)
- **`HorizontalFlipFilter`**: Mirror pixels horizontally around a center point
- **`VerticalFlipFilter`**: Mirror pixels vertically around a center point
- **`OffsetFilter`**: Move pixels by a specified offset
- **`ClipFilter`**: Limit drawing operations to a specified rectangular region

#### Visual Effect Filters
- **`TransparencyFilter`**: Make specific colors transparent
- **`MaskDiffFilter`**: Apply pixels only where mask differs from specified color
- **`MaskEqFilter`**: Apply pixels only where mask equals specified color
- **`AlphaFilter`**: Blend pixels with existing image using alpha transparency

#### Zoom and Scaling Filters
- **`PixelSpreadFilter`**: Scale pixels with defined center point and zoom factor
- **`PixelZoomFilter`**: Scale pixels with expansion, filling gaps between pixels

#### Gradient Filters
- **`GradientFilter2C`**: Apply a linear gradient between two colors

### Using Filters

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

## Text Rendering and Font Support

The library provides a comprehensive text rendering system using BDF fonts. Text rendering is implemented through the `uimg::TextSource` interface and the `uimg::TextPainter` abstract class. The concrete implementation `uimg::TextPainterForBdfFont` handles rendering for BDF fonts.

### Features
- Loading and rendering BDF fonts
- Multi-color text rendering
- Precise text positioning
- Support for text alignment
- Basic text formatting

### BDF Font Format

The library uses the BDF (Bitmap Distribution Format) font format:

**Format specifications:**
- [Wikipedia: Glyph Bitmap Distribution Format](https://en.wikipedia.org/wiki/Glyph_Bitmap_Distribution_Format)
- [Adobe BDF Specification](https://partners.adobe.com/public/developer/en/font/5005.BDF_Spec.pdf)
- [X11 Font Description](http://www.linuxfromscratch.org/blfs/view/svn/x/x7font.html)

**Font sources:**
- [X.org Font Archive](http://ftp.x.org/pub/individual/font/)
  - Example: `font-adobe-75dpi-1.0.0.tar.gz` contains `courR12.bdf`

Sample fonts are included in the `fonts/` directory.

### Text Demo Usage

```bash
./build/text_demo -font fonts/courR12.bdf -out text_output.ppm
```

## Getting Started Example

Here's a simple example to get you started:

```cpp
#include "uimg/images/RgbImage.h"
#include "uimg/images/PpmImageWriter.h"
#include "uimg/painters/PixelPainterForRgbImage.h"
#include "uimg/painters/LinePainter.h"

int main() {
    // Create a 400x300 RGB image
    uimg::RgbImage image(400, 300, uimg::RgbColor(255, 255, 255));
    
    // Create a painter for the image
    uimg::PixelPainterForRgbImage painter(image);
    
    // Draw a red line from (10,10) to (390,290)
    uimg::LinePainter linePainter(painter);
    linePainter.drawLine(
        uimg::Point(10, 10), 
        uimg::Point(390, 290), 
        uimg::RgbColor(255, 0, 0)
    );
    
    // Save to PPM file
    uimg::PpmImageWriter writer;
    writer.writeToFile(image, "output.ppm");
    
    return 0;
}
```

# Contributing

Contributions are welcome! Please ensure that:

1. All new features include appropriate demo applications
2. Code follows the existing style and patterns
3. Tests are included for new functionality
4. Documentation is updated accordingly

## Testing

Run the test suite with:

```bash
./run_tests.sh
```

Tests are located in the `tests/` directory and cover:
- Core functionality validation
- Logging system tests
- Type casting utilities
- Various API components

# License

See [LICENSE](LICENSE) file for license information.

# Related Projects

For more advanced plotting and charting capabilities, consider:

- [cpplot](https://github.com/thclark/cpplot) - C++ plotting library
- [JKQtPlotter](https://github.com/jkriege2/JKQtPlotter/) - Qt-based scientific plotting library

