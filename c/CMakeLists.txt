# Set version requirement
cmake_minimum_required(VERSION 3.16)

# Define project
project(argparse-c
    VERSION 0.0.1
    DESCRIPTION "CLI argument parser for C."
    LANGUAGES C)

# Set default to Debug
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()
message("-- ${PROJECT_NAME} Build type: ${CMAKE_BUILD_TYPE}")

set (SOURCES
    "argparse.c"
    "argparse.h"
)

foreach(FILE IN LISTS SOURCES)
    list(APPEND SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/${FILE}")
endforeach()

add_library(${PROJECT_NAME} ${SOURCES_LIST})

# Create list of all examples
set (EXAMPLES
    "examples/flags.c"
)

# Create target for each example
foreach(FILE IN LISTS EXAMPLES)
    get_filename_component(EXAMPLE_NAME ${FILE} NAME_WE)
    add_executable(${PROJECT_NAME}-${EXAMPLE_NAME} ${FILE})
    target_link_libraries(${PROJECT_NAME}-${EXAMPLE_NAME} ${PROJECT_NAME})
    target_include_directories(${PROJECT_NAME}-${EXAMPLE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
endforeach()
