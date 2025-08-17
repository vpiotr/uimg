cmake_minimum_required(VERSION 3.16)
project(chart3d_demo)

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

# Don't set output directories - let each executable build in its own directory
# We'll move them manually in the build script

# Include directories
include_directories(include)
include_directories(../common/include)
include_directories(../common/include/chart3d)
include_directories(../multi_chart3d/include)
include_directories(../../include)
include_directories(../../include/uimg)
include_directories(../../include/uimg/painters)
include_directories(../../include/uimg/utils)
include_directories(../../include/dlog)

add_executable(chart3d_demo
    main.cpp
    include/chart3d_demo.h
    include/chart_z_fxy_3d_sqrt.h
)

# Set the output name with _demo suffix
set_target_properties(chart3d_demo PROPERTIES OUTPUT_NAME "chart3d_demo")
