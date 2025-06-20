cmake_minimum_required(VERSION 3.16)
project(uimg)

set(CMAKE_CXX_STANDARD 17)

# Enable testing for CTest integration
enable_testing()

include_directories(apps/include)
include_directories(apps/include/samples)
include_directories(apps/include/samples/voronoi)
include_directories(apps/include/samples/mandelbrot)
include_directories(apps/include/samples/chart3d)
include_directories(apps/include/samples/draw_primitives)
include_directories(apps/include/samples/text_demo)
include_directories(apps/include/samples/filter_demo)
include_directories(include)
include_directories(include/uimg)
include_directories(include/uimg/painters)
include_directories(include/uimg/utils)
include_directories(include/ulog)

add_executable(voronoi
        LICENSE
        README.md
        apps/include/samples/voronoi/test_voronoi.h
        apps/include/samples/voronoi/voronoi.h
        apps/include/samples/test_painter_base.h
        apps/voronoi.cpp
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
        apps/include/samples/mandelbrot/test_mandelbrot.h
        apps/include/samples/mandelbrot/mandelbrot.h
        apps/include/samples/test_painter_base.h
        apps/mandelbrot.cpp
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
        apps/include/samples/chart3d/test_chart_z_fxy_3d_c.h
        apps/include/samples/chart3d/chart_z_fxy_3d.h
        apps/include/samples/test_painter_base.h
        apps/chart3d.cpp
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
        apps/include/samples/draw_primitives/test_draw_primitives.h
        apps/include/samples/test_painter_base.h
        apps/draw_primitives.cpp
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
        apps/include/samples/filter_demo/test_filter_demo.h
        apps/include/samples/test_painter_base.h
        apps/filter_demo.cpp
        include/uimg/painters/copier_for_pixels.h
        include/uimg/painters/painter_base.h
        include/uimg/painters/painter_for_pixels.h
        include/uimg/filters/filter_base.h
        include/uimg/filters/filter_for_pixels.h)

add_executable(2d_line_chart_demo
        LICENSE
        README.md
        apps/2d_line_chart_demo.cpp
        include/uimg/charts/chart.h
        include/uimg/charts/chart_styles.h
        include/uimg/charts/chart_renderer.h
        apps/include/samples/test_painter_base.h
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
        apps/text_demo.cpp
        apps/include/samples/text_demo/test_text_demo.h
        apps/include/samples/test_painter_base.h
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

# Test executable for comprehensive chart3d testing
add_executable(test_chart3d
        LICENSE
        README.md
        apps/test_chart3d.cpp
        apps/include/samples/test_chart3d.h
        apps/include/samples/test_logger.h
        apps/include/samples/chart3d/chart_z_fxy_3d.h
        apps/include/samples/chart3d/chart3d_renderer.h
        apps/include/samples/chart3d/chart3d_layout.h
        apps/include/samples/chart3d/multi_chart_3d.h
        apps/include/samples/chart3d/test_chart_z_fxy_3d_c.h
        apps/include/samples/test_painter_base.h
        apps/include/samples/logger.h
        include/utest/utest.h
        include/uimg/base/structs.h
        include/uimg/images/rgb_image.h
        include/uimg/images/ppm_image.h
        include/uimg/pixels/pixel_painter.h
        include/uimg/painters/painter_base.h
        include/uimg/painters/painter_for_pixels.h
        include/uimg/painters/painter_for_rgb_image.h
        include/uimg/painters/antialiased_painter_for_pixels.h
        include/uimg/painters/rect_painter.h
        include/uimg/filters/filter_base.h
        include/uimg/filters/pixel_tracing_filter.h
        include/uimg/utils/algorithm.h
        include/uimg/utils/color_utils.h
        include/uimg/utils/cubic_spline_utils.h
        include/uimg/utils/math_utils.h
        include/uimg/utils/point_utils.h)

add_executable(observers_demo
        LICENSE
        README.md
        apps/observers_demo.cpp
        include/ulog/ulog.h
        include/ulog/log_output.h)

add_executable(basic_logging_demo
        LICENSE
        README.md
        apps/basic_logging_demo.cpp
        include/ulog/ulog.h
        include/ulog/log_output.h)

# Add tests for CTest integration
add_test(NAME Chart3DComprehensiveTest 
         COMMAND test_chart3d)

# Set test properties
set_tests_properties(Chart3DComprehensiveTest PROPERTIES
    TIMEOUT 120
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)


