cmake_minimum_required(VERSION 3.27)
project(auxts C)

set(CMAKE_C_STANDARD 17)

enable_testing()

add_test(rfc_test auxts "rfc")

add_test(murmur3_test auxts "murmur3")

add_executable(auxts main.c
        src/endian.h
        src/macro.h
        src/rfc.h
        src/rfc.c
        src/murmur3.h
        src/murmur3.c)
