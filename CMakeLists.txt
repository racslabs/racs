# RACS - Remote Audio Caching Server
# Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
#
# Licensed under the RACS Source Available License (RACS-SAL-1.0).
# Non-commercial use only. Commercial use requires a paid license.
# Contact: sales@racslabs.com
#
# SPDX-License-Identifier: RACS-SAL-1.0

cmake_minimum_required(VERSION 3.22.1)
project(racs LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

set(RACS_TEST_DIR ${CMAKE_SOURCE_DIR}/test)
set(RACS_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Guile REQUIRED)
find_package(Msgpack)
find_package(Lame)
find_package(Opus)
find_package(OpusEnc)
find_package(Yaml)

include_directories(
        ${OPUS_INCLUDE_DIRS}
        ${OPUSENC_INCLUDE_DIRS}
        ${LAME_INCLUDE_DIRS}
        ${GUILE_INCLUDE_DIRS}
        ${YAML_INCLUDE_DIRS}
        ${MSGPACK_INCLUDE_DIRS}
)

file(GLOB_RECURSE RACS_TEST_FILES CONFIGURE_DEPENDS ${RACS_TEST_DIR}/*.hpp ${RACS_TEST_DIR}/*.cpp)
file(GLOB_RECURSE RACS_SOURCE_FILES CONFIGURE_DEPENDS ${RACS_SOURCE_DIR}/*.h ${RACS_SOURCE_DIR}/*.c)

add_executable(racs ${RACS_SOURCE_FILES}
        src/queue.h
        src/queue.c)

target_link_libraries(racs
        ${MSGPACK_LIBRARIES}
        ${GUILE_LIBRARIES}
        ${LAME_LIBRARIES}
        ${OPUS_LIBRARIES}
        ${OPUSENC_LIBRARIES}
        ${YAML_LIBRARIES}
)

include(FetchContent)
include(GoogleTest)

if (WIN32 OR APPLE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
            DOWNLOAD_EXTRACT_TIMESTAMP true
    )
else()
    FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
endif()


FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(racs_test ${RACS_SOURCE_FILES} ${RACS_TEST_FILES})

target_link_libraries(
        racs_test
        PRIVATE
        GTest::gtest_main
)

gtest_discover_tests(racs_test)

option(INSTALL_GMOCK OFF)
option(INSTALL_GTEST OFF)

enable_testing()