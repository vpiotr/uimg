cmake_minimum_required(VERSION 3.16)
project(cast_demo)

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
# Build executables in their own directories - moved by build script
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)

# Include directories
include_directories(../../include)
include_directories(../../include/dlog)
include_directories(../../include/uimg/utils)

add_executable(cast_demo
    main.cpp
)
