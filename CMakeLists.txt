cmake_minimum_required(VERSION 3.28)
project(ParseJSONC C)

set(CMAKE_C_STANDARD 23)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/libs)

file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.c")
file(GLOB_RECURSE LIBS "${LIB_DIR}/*.a")

add_executable(
    parse main.c
        ${SOURCES}
)

target_link_libraries(
    parse
        ${LIBS} raylib
)
