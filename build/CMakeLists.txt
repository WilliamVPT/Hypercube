cmake_minimum_required(VERSION 3.22.1)
project(hypercube)

set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/../")

set(SOURCES
    ../sommet.c
    ../token.c
    ../hypercube.c
)


add_library(util STATIC ${SOURCES})

add_executable(hypercube ${EXECUTABLE_OUTPUT_PATH} ../hypercube.c)
add_dependencies(hypercube util)

target_link_libraries(hypercube util)
target_link_libraries(hypercube m)
