# RACS - Remote Audio Caching Server
# Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
#
# Licensed under the RACS Source Available License (RACS-SAL-1.0).
# Non-commercial use only. Commercial use requires a paid license.
# Contact: sales@racslabs.com
#
# SPDX-License-Identifier: RACS-SAL-1.0

cmake_minimum_required(VERSION 3.22.1)
project(racs LANGUAGES C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

set(RACS_TEST_DIR ${CMAKE_SOURCE_DIR}/test)
set(RACS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#---------Uncomment For Debugging Only----------#
#option(ENABLE_ASAN "Enable AddressSanitizer" ON)
#
#if(ENABLE_ASAN)
#    add_compile_options(
#            -fsanitize=address
#            -fno-omit-frame-pointer
#            -g
#            -O0
#    )
#    add_link_options(-fsanitize=address)
#endif()

find_package(Guile REQUIRED)
find_package(Msgpack)
find_package(Lame)
find_package(Opus)
find_package(OpusEnc)
find_package(Yaml)
find_package(Event)
find_package(Zstd)

include_directories(
        ${OPUS_INCLUDE_DIRS}
        ${OPUSENC_INCLUDE_DIRS}
        ${LAME_INCLUDE_DIRS}
        ${GUILE_INCLUDE_DIRS}
        ${YAML_INCLUDE_DIRS}
        ${MSGPACK_INCLUDE_DIRS}
        ${EVENT_INCLUDE_DIRS}
        ${ZSTD_INCLUDE_DIRS}
)

file(GLOB_RECURSE RACS_TEST_FILES CONFIGURE_DEPENDS ${RACS_TEST_DIR}/*.hpp ${RACS_TEST_DIR}/*.cpp)
file(GLOB_RECURSE RACS_SOURCE_FILES CONFIGURE_DEPENDS ${RACS_SOURCE_DIR}/*.h ${RACS_SOURCE_DIR}/*.c)

add_executable(racs ${RACS_SOURCE_FILES})

target_link_libraries(racs
        ${MSGPACK_LIBRARIES}
        ${GUILE_LIBRARIES}
        ${LAME_LIBRARIES}
        ${OPUS_LIBRARIES}
        ${OPUSENC_LIBRARIES}
        ${YAML_LIBRARIES}
        ${EVENT_LIBRARIES}
        ${ZSTD_LIBRARIES}
        uuid
)