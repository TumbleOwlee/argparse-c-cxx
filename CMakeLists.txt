# Set version requirement
cmake_minimum_required(VERSION 3.16)

# Define project
project(argparse
    VERSION 0.0.1
    DESCRIPTION "CLI argument parser for C/C++."
    LANGUAGES C CXX)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/c")
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/cxx")
