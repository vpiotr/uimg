# CMakeLists.txt for dlog tests
cmake_minimum_required(VERSION 3.10)

# Set project name
project(dlog_tests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(../include)
include_directories(../demos/include)

# Set output directory for test executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../build/tests)

# Test executables
set(TEST_SOURCES
    dlog/test_dlog_basic.cpp
    dlog/test_dlog_features.cpp
    utils/test_unsigned_cast_basic.cpp
    utils/test_unsigned_cast_failures.cpp
    chart3d/test_multi_chart3d_boundaries_simple.cpp
)

# Create test executables
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
endforeach()

# Create a custom target to run all tests
add_custom_target(run_all_tests
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/run_tests.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running all dlog tests"
)

# Make run_all_tests depend on all test executables
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_dependencies(run_all_tests ${TEST_NAME})
endforeach()

# Individual test targets
foreach(TEST_SOURCE ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
    add_custom_target(run_${TEST_NAME}
        COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_NAME}
        DEPENDS ${TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running ${TEST_NAME}"
    )
endforeach()

# Create output directory
file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/output)
