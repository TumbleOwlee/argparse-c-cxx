# Set version requirement
cmake_minimum_required(VERSION 3.16)

# Define project
project(argparse
    VERSION 0.0.1
    DESCRIPTION "CLI argument parser for C++20."
    LANGUAGES C)

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set default to Debug
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE Debug)
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

set (SOURCES
    "argparse.c"
#    "argparse.h"
)

foreach(FILE IN LISTS SOURCES)
    list(APPEND SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/${FILE}")
endforeach()

add_library(${PROJECT_NAME}-c ${SOURCES_LIST})

# Create list of all examples
set (EXAMPLES
    "examples/flags.c"
)

# Create target for each test
foreach(FILE IN LISTS EXAMPLES)
    get_filename_component(EXAMPLE_NAME ${FILE} NAME_WE)
    add_executable(${PROJECT_NAME}-c-${EXAMPLE_NAME} ${FILE})
    target_link_libraries(${PROJECT_NAME}-c-${EXAMPLE_NAME} ${PROJECT_NAME}-c)
    target_include_directories(${PROJECT_NAME}-c-${EXAMPLE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
endforeach()

#set (SOURCES
#    "argparse.cxx"
#    "argparse.hxx"
#)
#
#foreach(FILE IN LISTS SOURCES)
#    list(APPEND SOURCES_LIST "${CMAKE_CURRENT_SOURCE_DIR}/src/${FILE}")
#endforeach()
#
#add_library(${PROJECT_NAME}-cxx ${SOURCES_LIST})
#set_property(TARGET ${PROJECT_NAME}-cxx PROPERTY CXX_STANDARD 20)
#
## Create list of all examples
#set (EXAMPLES
#    "examples/flags.cxx"
#    "examples/commands.cxx"
#)
#
## Create target for each test
#foreach(FILE IN LISTS EXAMPLES)
#    get_filename_component(EXAMPLE_NAME ${FILE} NAME_WE)
#    add_executable(${PROJECT_NAME}-cxx-${EXAMPLE_NAME} ${FILE})
#    target_link_libraries(${PROJECT_NAME}-cxx-${EXAMPLE_NAME} ${PROJECT_NAME}-cxx)
#    set_property(TARGET ${PROJECT_NAME}-cxx-${EXAMPLE_NAME} PROPERTY CXX_STANDARD 20)
#    target_include_directories(${PROJECT_NAME}-cxx-${EXAMPLE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
#endforeach()
