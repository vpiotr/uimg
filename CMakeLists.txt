cmake_minimum_required(VERSION 3.16)
project(uimg)

set(CMAKE_CXX_STANDARD 17)

# Enable comprehensive warning flags to match Windows behavior
if(MSVC)
    # Windows (MSVC) - Enable most warnings including conversion warnings
    add_compile_options(/W4 /WX- /wd4996)
else()
    # Linux/GCC or Clang - Enable comprehensive warnings including conversion warnings
    add_compile_options(
        -Wall                    # Enable most warnings
        -Wextra                  # Enable extra warnings
        -Wconversion             # Warn about type conversions that may alter values
        -Wsign-conversion        # Warn about sign conversions
        -Wfloat-conversion       # Warn about float conversions
        -Wcast-qual              # Warn about casts that remove qualifiers
        -Wcast-align             # Warn about casts that increase alignment requirements
        -Wshadow                 # Warn about variable shadowing
        -Wpedantic               # Warn about non-standard C++
        -Wunused                 # Warn about unused variables/functions
        -Wuninitialized          # Warn about uninitialized variables
    )
endif()

# Enable testing for CTest integration
enable_testing()

include_directories(demos/include)
include_directories(demos/include/samples)
include_directories(demos/include/samples/voronoi)
include_directories(demos/include/samples/mandelbrot)
include_directories(demos/include/samples/chart3d)
include_directories(demos/include/samples/draw_primitives)
include_directories(demos/include/samples/text_demo)
include_directories(demos/include/samples/filter_demo)
include_directories(include)
include_directories(include/uimg)
include_directories(include/uimg/painters)
include_directories(include/uimg/utils)
include_directories(include/ulog)

add_executable(voronoi
        LICENSE
        README.md
        demos/include/samples/voronoi/voronoi_demo.h
        demos/include/samples/voronoi/voronoi.h
        demos/include/samples/demo_painter_base.h
        demos/voronoi.cpp
        include/uimg/painters/copier_for_pixels.h
        include/uimg/painters/painter_base.h
        include/uimg/painters/painter_for_pixels.h
        include/uimg/painters/painter_for_rgb_image.h
        include/uimg/images/ppm_image.h
        include/uimg/images/rgb_image.h
        include/uimg/utils/algorithm.h
        include/uimg/utils/color_utils.h
        include/uimg/utils/cubic_spline_utils.h
        include/uimg/utils/math_utils.h
        include/uimg/utils/point_utils.h include/uimg/pixels/pixel_painter.h include/uimg/pixels/pixel_source.h include/uimg/base/structs.h include/uimg/images/pixel_image.h)

add_executable(mandelbrot
        LICENSE
        README.md
        demos/include/samples/mandelbrot/mandelbrot_demo.h
        demos/include/samples/mandelbrot/mandelbrot.h
        demos/include/samples/demo_painter_base.h
        demos/mandelbrot.cpp
        include/uimg/painters/copier_for_pixels.h
        include/uimg/painters/painter_base.h
        include/uimg/painters/painter_for_pixels.h
        include/uimg/painters/painter_for_rgb_image.h
        include/uimg/images/ppm_image.h
        include/uimg/images/rgb_image.h
        include/uimg/utils/algorithm.h
        include/uimg/utils/color_utils.h
        include/uimg/utils/cubic_spline_utils.h
        include/uimg/utils/math_utils.h
        include/uimg/utils/point_utils.h include/uimg/pixels/pixel_painter.h include/uimg/pixels/pixel_source.h include/uimg/base/structs.h include/uimg/images/pixel_image.h)

add_executable(chart3d
        LICENSE
        README.md
        demos/include/chart3d/test_chart_z_fxy_3d_c.h
        demos/include/chart3d/chart_z_fxy_3d.h
        demos/include/samples/test_painter_base.h
        demos/chart3d.cpp
        include/uimg/painters/copier_for_pixels.h
        include/uimg/painters/painter_base.h
        include/uimg/painters/painter_for_pixels.h
        include/uimg/painters/painter_for_rgb_image.h
        include/uimg/painters/rect_painter.h
        include/uimg/painters/antialiased_painter_for_pixels.h
        include/uimg/images/ppm_image.h
        include/uimg/images/rgb_image.h
        include/uimg/utils/algorithm.h
        include/uimg/utils/color_utils.h
        include/uimg/utils/cubic_spline_utils.h
        include/uimg/utils/math_utils.h
        include/uimg/utils/point_utils.h
        include/uimg/pixels/pixel_painter.h
        include/uimg/pixels/pixel_source.h
        include/uimg/base/structs.h
        include/uimg/images/pixel_image.h)
        
add_executable(draw_primitives
        LICENSE
        README.md
        demos/include/samples/draw_primitives/draw_primitives_demo.h
        demos/include/samples/demo_painter_base.h
        demos/draw_primitives.cpp
        include/uimg/painters/copier_for_pixels.h
        include/uimg/painters/painter_base.h
        include/uimg/painters/painter_for_pixels.h
        include/uimg/painters/painter_for_rgb_image.h
        include/uimg/images/ppm_image.h
        include/uimg/images/rgb_image.h
        include/uimg/utils/algorithm.h
        include/uimg/utils/color_utils.h
        include/uimg/utils/cubic_spline_utils.h
        include/uimg/utils/math_utils.h
        include/uimg/utils/point_utils.h include/uimg/pixels/pixel_painter.h include/uimg/pixels/pixel_source.h include/uimg/base/structs.h include/uimg/images/pixel_image.h)

add_executable(filter_demo
        LICENSE
        README.md
        demos/include/samples/filter_demo/test_filter_demo.h
        demos/include/samples/test_painter_base.h
        demos/filter_demo.cpp
        include/uimg/painters/copier_for_pixels.h
        include/uimg/painters/painter_base.h
        include/uimg/painters/painter_for_pixels.h
        include/uimg/filters/filter_base.h
        include/uimg/filters/filter_for_pixels.h)

add_executable(2d_line_chart_demo
        LICENSE
        README.md
        demos/2d_line_chart_demo.cpp
        include/uimg/charts/chart.h
        include/uimg/charts/chart_styles.h
        include/uimg/charts/chart_renderer.h
        demos/include/samples/test_painter_base.h
        include/uimg/base/structs.h
        include/uimg/images/rgb_image.h
        include/uimg/images/ppm_image.h
        include/uimg/pixels/pixel_painter.h
        include/uimg/painters/painter_base.h
        include/uimg/painters/painter_for_pixels.h
        include/uimg/painters/painter_for_rgb_image.h
        include/uimg/fonts/bdf_font.h
        include/uimg/fonts/painter_for_bdf_font.h
        include/uimg/text/text_painter.h
        include/uimg/text/text_base.h
        include/uimg/utils/color_utils.h
        include/uimg/utils/math_utils.h
        include/uimg/utils/point_utils.h)

add_executable(text_demo
        LICENSE
        README.md
        demos/text_demo.cpp
        demos/include/samples/text_demo/test_text_demo.h
        demos/include/samples/test_painter_base.h
        include/uimg/base/structs.h
        include/uimg/images/rgb_image.h
        include/uimg/images/ppm_image.h
        include/uimg/pixels/pixel_painter.h
        include/uimg/painters/painter_base.h
        include/uimg/painters/painter_for_pixels.h
        include/uimg/painters/painter_for_rgb_image.h
        include/uimg/fonts/bdf_font.h
        include/uimg/fonts/painter_for_bdf_font.h
        include/uimg/text/text_painter.h
        include/uimg/utils/color_utils.h
        include/uimg/utils/math_utils.h
        include/uimg/utils/point_utils.h)

add_executable(observers_demo
        LICENSE
        README.md
        demos/observers_demo.cpp
        include/dlog/dlog.h)

add_executable(basic_logging_demo
        LICENSE
        README.md
        demos/basic_logging_demo.cpp
        include/dlog/dlog.h)

add_executable(cast_demo
        LICENSE
        README.md
        demos/cast_demo.cpp
        include/dlog/dlog.h
        include/uimg/utils/cast.h)

add_executable(cast_validation_disabled_demo
        LICENSE
        README.md
        demos/cast_validation_disabled_demo.cpp
        include/dlog/dlog.h
        include/uimg/utils/cast.h)

