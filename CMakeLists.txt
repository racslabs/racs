cmake_minimum_required(VERSION 3.27)
project(racs C)

set(CMAKE_C_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Guile REQUIRED)
find_package(msgpack-c REQUIRED)
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
        ${msgpack-c_INCLUDE_DIRS})

option(RACS_AVX2_CMAKE_BUILD "AVX2 CMake build" OFF)
option(RACS_NEON8_CMAKE_BUILD "NEON8 CMake build" ON)

if (RACS_AVX2_CMAKE_BUILD)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-march=native -O2)
    endif()
elseif (RACS_NEON8_CMAKE_BUILD)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(ARM_ALIASES armv6l armv7l aarch64)
        add_compile_options(-O2 -march=armv8-a+simd)
    endif()
endif()

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
        src/simd/simd.h
        src/parser.h
        src/parser.c
        src/exec.h
        src/exec.c
        src/command.h
        src/command.c
        src/serialization.h
        src/serialization.c
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
        src/simd/pcm_simd.h
        src/simd/pcm_neon.c
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
        src/conn.h
        src/conn.c
        src/server.h
        src/server.c
        src/config.h
        src/log.h
        src/log.c
        src/version.h
)

target_link_libraries(racs msgpack-c
        ${GUILE_LIBRARIES}
        ${LAME_LIBRARIES}
        ${OPUS_LIBRARIES}
        ${OPUSENC_LIBRARIES}
        ${YAML_LIBRARIES})

enable_testing()