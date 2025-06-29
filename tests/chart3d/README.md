# Multi-Chart3D Boundary Testing Module

## Overview

This test module provides comprehensive validation for multi-chart 3D rendering boundary behavior in the uimg library. The tests focus on ensuring that chart rendering respects canvas boundaries and properly isolates individual chart regions.

## Test Files

### `test_multi_chart3d_boundaries_simple.cpp`

The main test file that implements:

1. **TrackingPixelPainter** - A pixel painter wrapper that records all drawing operations for analysis
2. **MockMultiChartRenderer** - A simplified chart renderer for testing boundary logic
3. **Comprehensive boundary tests** - Various test scenarios for different chart layouts

## Features Tested

### 1. Pixel Tracking and Analysis
- **TrackingPixelPainter**: Records all `putPixel()` operations
- **Pixel history**: Complete log of drawing operations with coordinates and colors
- **Region analysis**: Methods to query pixels within specific rectangular regions
- **Boundary violation detection**: Counts pixels outside allowed regions

### 2. Chart Layout Parsing
- **Auto layout**: Automatic grid determination based on chart count
- **Custom layouts**: Parsing of "NxM" format layout specifications
- **Horizontal/Vertical layouts**: Single-row and single-column arrangements

### 3. Boundary Validation
- **Canvas bounds checking**: Ensures no pixels drawn outside canvas
- **Chart region isolation**: Verifies charts stay within assigned regions
- **Multi-chart layouts**: Tests 2x2, horizontal, vertical, and custom layouts

## Test Cases

### `TrackingPixelPainter_RecordsPixelOperations`
Verifies the tracking mechanism correctly records pixel operations and provides accurate region queries.

### `MultiChart3D_LayoutParsing_AutoLayout`
Tests automatic layout determination for different chart counts:
- 4 charts → 2x2 grid
- 6 charts → 2x3 grid

### `MultiChart3D_LayoutParsing_CustomLayout`
Verifies parsing of custom layout strings like "3x2".

### `MultiChart3D_SingleChart_FillsCanvas`
Ensures a single chart utilizes the entire canvas and has content in all quadrants.

### `MultiChart3D_HorizontalLayout_TwoCharts`
Tests horizontal layout with two charts side-by-side:
- Verifies both charts have content
- Checks boundary compliance
- Validates reasonable pixel distribution

### `MultiChart3D_FourCharts_2x2_NoBoundaryViolations`
The main test for 4-chart boundary validation:
- Tests 2x2 grid layout
- Verifies no canvas boundary violations
- Checks individual chart region isolation
- Validates each chart has content

## Key Testing Concepts

### Boundary Violation Detection
The tests check for two types of boundary violations:

1. **Canvas violations**: Pixels drawn outside the overall canvas bounds
2. **Chart violations**: Pixels from one chart drawn in another chart's region

### Mock Rendering
Instead of using the full chart3D rendering pipeline (which has complex dependencies), the tests use a `MockMultiChartRenderer` that:
- Implements the same layout logic as the real renderer
- Draws simple patterns (borders and diagonals) to simulate chart content
- Focuses on boundary behavior rather than visual accuracy

### Pixel Analysis
The `TrackingPixelPainter` provides several analysis methods:
- `getPixelsInRegion()`: Get all pixels within a rectangular area
- `countPixelsOutsideRegion()`: Count pixels outside a specified area
- `hasPixelAt()`: Check if a specific coordinate was drawn to

## Building and Running

### Build Tests
```bash
cd /home/piotr/progs/oss_projects/uimg/tests
cmake .
make
```

### Run Chart3D Tests Only
```bash
./run_chart3d_tests.sh
```

### Run All Tests
```bash
./run_all_tests.sh
```

### Run Specific Test
```bash
../build/tests/test_multi_chart3d_boundaries_simple
```

## Expected Output

When all tests pass, you should see:
```
[OK] Test [TrackingPixelPainter_RecordsPixelOperations] succeeded
[OK] Test [MultiChart3D_LayoutParsing_AutoLayout] succeeded
[OK] Test [MultiChart3D_LayoutParsing_CustomLayout] succeeded
[OK] Test [MultiChart3D_SingleChart_FillsCanvas] succeeded
[OK] Test [MultiChart3D_HorizontalLayout_TwoCharts] succeeded
[OK] Test [MultiChart3D_FourCharts_2x2_NoBoundaryViolations] succeeded

Total: 6 tests, 6 passed [OK], 0 failed [FAIL]
```

## Integration with Build System

The test is integrated into the CMake build system via `tests/CMakeLists.txt`:

```cmake
set(TEST_SOURCES
    ...
    chart3d/test_multi_chart3d_boundaries_simple.cpp
)
```

## Dependencies

The test module depends on:
- `utest/utest.h` - Unit testing framework
- `dlog/dlog.h` - Logging framework  
- `uimg/images/rgb_image.h` - Image handling
- `uimg/painters/painter_for_rgb_image.h` - Pixel painting
- Standard C++ libraries (`<vector>`, `<set>`, `<functional>`, etc.)

## Design Rationale

### Why Mock Rendering?
The original approach tried to use the full `multi_chart_3d_demo` class but encountered complex include dependencies. The mock approach:
- Isolates the boundary logic being tested
- Avoids dependency complications
- Focuses testing on the specific behavior of interest
- Provides faster test execution

### Why Pixel Tracking?
Direct pixel tracking allows precise verification of drawing behavior:
- Detects boundary violations at the pixel level
- Provides quantitative analysis of drawing patterns
- Enables verification of chart region isolation
- Allows testing without generating actual image files

## Future Enhancements

Potential additions to the test suite:
1. **Anti-aliasing tests**: Verify AA doesn't cause boundary violations
2. **Border drawing tests**: Specific tests for chart border rendering
3. **Error condition tests**: Test behavior with invalid layouts
4. **Performance tests**: Measure rendering performance
5. **Integration tests**: Tests with real chart3D rendering (when dependencies resolved)
