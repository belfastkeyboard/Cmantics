cmake_minimum_required(VERSION 3.28)
project(ParseINIC C)

set(CMAKE_C_STANDARD 23)

add_executable(ParseINIC
                main.c arena.c ini.c)
