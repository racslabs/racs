# RACS - Remote Audio Caching Server
# Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
#
# Licensed under the RACS Source Available License (RSAL-1.0).
# Non-commercial use only. Commercial use requires a paid license.
# Contact: sales@racslabs.com
#
# SPDX-License-Identifier: RACS-SAL-1.0

cmake_minimum_required(VERSION 3.27)
project(racs LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)

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
        ${RACS_DSP_SOURCE_DIR}
)

add_executable(racs
        src/export.h
        src/time.h
        src/time.c
        src/murmur3.h
        src/murmur3.c
        src/memtable.h
        src/memtable.c
        src/cache.h
        src/cache.c
        src/result.h
        src/result.c
        src/filelist.h
        src/filelist.c
        src/bytes.h
        src/kvstore.h
        src/kvstore.c
        src/parser.h
        src/parser.c
        src/exec.h
        src/exec.c
        src/command.h
        src/command.c
        src/pack.h
        src/pack.c
        src/db.h
        src/db.c
        src/context.h
        src/context.c
        src/scm.h
        src/scm.c
        src/scm_bindings.h
        src/scm_bindings.c
        src/crc32c.h
        src/crc32c.c
        src/frame.h
        src/frame.c
        src/pcm.h
        src/types.h
        src/memory.h
        src/memory.c
        src/pcm.c
        src/extract.h
        src/extract.c
        src/wav.h
        src/wav.c
        src/format.c
        src/format.h
        src/mp3.h
        src/mp3.c
        src/ogg.h
        src/ogg.c
        src/streaminfo.h
        src/streaminfo.c
        src/stream.h
        src/stream.c
        src/server.h
        src/server.c
        src/config.h
        src/log.h
        src/log.c
        src/version.h
        src/types.c
)

target_link_libraries(racs
        ${MSGPACK_LIBRARIES}
        ${GUILE_LIBRARIES}
        ${LAME_LIBRARIES}
        ${OPUS_LIBRARIES}
        ${OPUSENC_LIBRARIES}
        ${YAML_LIBRARIES}
)

set(RACS_TEST_DIR ${CMAKE_SOURCE_DIR}/test)

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

if(WIN32)
    set(CMAKE_STATIC_LIBRARY_SUFFIX ".lib")
elseif(UNIX)
    set(CMAKE_STATIC_LIBRARY_SUFFIX ".a")
endif()

FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE RACS_TEST_FILES CONFIGURE_DEPENDS ${RACS_TEST_DIR}/*.hpp ${RACS_TEST_DIR}/*.cpp)

set_target_properties(${RACS_STATIC_LIBRARY_NAME} PROPERTIES PREFIX "")

enable_testing()

add_executable(racs_test ${RACS_SOURCE_FILES} ${RACS_TEST_FILES})

target_link_libraries(
        racs_test
        PRIVATE
        GTest::gtest_main
        ${RACS_STATIC_LIBRARY_NAME}
)

gtest_discover_tests(racs_test)

option(INSTALL_GMOCK OFF)
option(INSTALL_GTEST OFF)

enable_testing()