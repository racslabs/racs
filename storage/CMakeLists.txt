cmake_minimum_required(VERSION 3.27)
project(auxts C)

set(CMAKE_C_STANDARD 17)

add_executable(auxts main.c
        src/rfc.h
        src/rfc.c)
