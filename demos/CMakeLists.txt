cmake_minimum_required(VERSION 3.16)
project(uimg_demos)

set(CMAKE_CXX_STANDARD 17)

# Enable comprehensive warning flags to match parent project
if(MSVC)
    add_compile_options(/W4 /WX- /wd4996)
else()
    add_compile_options(
        -Wall -Wextra -Wconversion -Wsign-conversion -Wfloat-conversion
        -Wcast-qual -Wcast-align -Wshadow -Wpedantic -Wunused -Wuninitialized
    )
endif()

# Set build directories to root project build folder
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/../build/demos)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Create build/demos directory for output
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR})

# Add all demo subdirectories
add_subdirectory(2d_line_chart)
add_subdirectory(basic_logging)
add_subdirectory(cast)
add_subdirectory(cast_validation_disabled)
add_subdirectory(chart3d)
add_subdirectory(draw_primitives)
add_subdirectory(filters)
add_subdirectory(mandelbrot)
add_subdirectory(multi_chart3d)
add_subdirectory(observers)
add_subdirectory(text)
add_subdirectory(voronoi)
