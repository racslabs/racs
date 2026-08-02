# RACS - Remote Audio Caching Server
# Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
#
# Licensed under the RACS Source Available License (RACS-SAL-1.0).
# Non-commercial use only. Commercial use requires a paid license.
# Contact: sales@racslabs.com
#
# SPDX-License-Identifier: RACS-SAL-1.0

cmake_minimum_required(VERSION 3.22.1)
project(racs C)

set(CMAKE_C_STANDARD 17)
set(RACS_TEST_DIR ${CMAKE_SOURCE_DIR}/test)
set(RACS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(UNITY_SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/unity/src)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#---------Uncomment For Debugging Only----------#
option(ENABLE_ASAN "Enable AddressSanitizer" ON)

if(ENABLE_ASAN)
    add_compile_options(
            -fsanitize=address
            -fno-omit-frame-pointer
            -g
            -O0
    )
    add_link_options(-fsanitize=address)
endif()

find_package(Msgpack)
find_package(Lame)
find_package(Opus)
find_package(OpusEnc)
find_package(Yaml)
find_package(Event)
find_package(Zstd)
find_package(Mpg123)

set(RACS_LIBRARIES
        ${MSGPACK_LIBRARIES}
        ${LAME_LIBRARIES}
        ${OPUS_LIBRARIES}
        ${OPUSENC_LIBRARIES}
        ${YAML_LIBRARIES}
        ${EVENT_LIBRARIES}
        ${ZSTD_LIBRARIES}
	${MPG123_LIBRARIES}
        uuid
)

include_directories(
        ${OPUS_INCLUDE_DIRS}
        ${OPUSENC_INCLUDE_DIRS}
        ${LAME_INCLUDE_DIRS}
        ${GUILE_INCLUDE_DIRS}
        ${YAML_INCLUDE_DIRS}
        ${MSGPACK_INCLUDE_DIRS}
        ${EVENT_INCLUDE_DIRS}
        ${ZSTD_INCLUDE_DIRS}
	${MPG123_INCLUDE_DIRS}
)

file(GLOB_RECURSE RACS_TEST_FILES CONFIGURE_DEPENDS ${RACS_TEST_DIR}/*.h ${RACS_TEST_DIR}/*.c)
file(GLOB_RECURSE RACS_SOURCE_FILES CONFIGURE_DEPENDS ${RACS_SOURCE_DIR}/*.h ${RACS_SOURCE_DIR}/*.c)
file(GLOB_RECURSE UNITY_SOURCE_FILES CONFIGURE_DEPENDS ${UNITY_SOURCE_DIR}/*.h ${UNITY_SOURCE_DIR}/*.c)

list(FILTER RACS_SOURCE_FILES EXCLUDE REGEX "racs_cli.c")

add_executable(racs ${RACS_SOURCE_FILES} src/racs_cli.c)

target_link_libraries(racs ${RACS_LIBRARIES})

enable_testing()

add_executable(racs_test
        ${RACS_SOURCE_FILES}
        ${RACS_TEST_FILES}
        ${UNITY_SOURCE_FILES}
)

target_include_directories(racs_test PRIVATE
        ${RACS_SOURCE_DIR}
        ${UNITY_SOURCE_DIR}
)
target_link_libraries(racs_test ${RACS_LIBRARIES})

configure_file(${RACS_TEST_DIR}/test-conf.yaml
        ${CMAKE_CURRENT_BINARY_DIR}/test-conf.yaml
        COPYONLY)

configure_file(${RACS_TEST_DIR}/chopin.mp3
        ${CMAKE_CURRENT_BINARY_DIR}/chopin.mp3
        COPYONLY)

add_test(NAME AllTests COMMAND racs_test)
