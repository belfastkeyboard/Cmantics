cmake_minimum_required(VERSION 3.28)

project(Cmantics C)

set(CMAKE_C_STANDARD 23)


# uncomment to compile release build, which removes asserts and compiles with optimisation
#set(BUILD Release)


set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/libs)

file(GLOB_RECURSE SOURCES "${SOURCE_DIR}/*.c")
file(GLOB_RECURSE LIBS "${LIB_DIR}/*.a")


if (BUILD STREQUAL "Release")
    message(STATUS "Release build: enabling optimizations and defining NDEBUG")

    add_compile_definitions(NDEBUG)

    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -funroll-loops -ffast-math")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -funroll-loops -ffast-math")
endif()


add_library(
    Cmantics ${SOURCES}
)

#add_executable(Cmantics main.c ${SOURCES})
