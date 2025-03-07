cmake_minimum_required(VERSION 3.27)
project(auxts C)

set(CMAKE_C_STANDARD 17)

enable_testing()

find_library(FLAC_LIBRARY NAMES FLAC)
find_path(FLAC_INCLUDE_DIR NAMES FLAC/all.h)
include_directories(${FLAC_INCLUDE_DIR})

find_package(msgpack-c CONFIG REQUIRED)

option(AUXTS_AVX2_CMAKE_BUILD "AVX2 CMake build" OFF)
option(AUXTS_NEON8_CMAKE_BUILD "NEON8 CMake build" ON)

if (AUXTS_AVX2_CMAKE_BUILD)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-march=native -O2)
    endif()
elseif (AUXTS_NEON8_CMAKE_BUILD)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(ARM_ALIASES armv6l armv7l aarch64)
        add_compile_options(-O2 -march=armv8-a+simd)
    endif()
endif()

add_executable(auxts main.c
        src/endian.h
        src/export.h
        src/timestamp.h
        src/timestamp.c
        src/murmur3.h
        src/murmur3.c
        src/memtable.h
        src/memtable.c
        src/cache.h
        src/cache.c
        src/extract.h
        src/extract.c
        src/flac.h
        src/flac.c
        src/auxts.h
        src/auxts.c
        src/filelist.h
        src/filelist.c
        src/bytes.h
        src/kvstore.h
        src/kvstore.c
        src/simd/simd_neon.h
        src/simd/simd.h
        src/simd/simd_neon-i32.c
        src/simd/simd_neon-i16.c
        tests/test_runner.h
        tests/cache_test.h
        tests/cache_test.c
        tests/extract_test.h
        tests/extract_test.c
        tests/memtable_test.h
        tests/memtable_test.c
        tests/kvstore_test.h
        tests/kvstore_test.c
        tests/murmur3_test.h
        tests/murmur3_test.c
        tests/timestamp_test.h
        tests/timestamp_test.c
        tests/simd_test.h
        tests/simd_test.c
        src/parser.h
        src/parser.c
        tests/parser_test.h
        tests/parser_test.c
        src/command_exec.h
        src/command_exec.c
        src/commands.h
        src/commands.c
        src/serialization.h
        src/serialization.c
        tests/test_utils.h
)

target_include_directories(auxts PUBLIC ${msgpack-c_INCLUDE_DIRS})
target_link_libraries(auxts msgpack-c ${FLAC_LIBRARY})

# Unit tests
add_test(test_lru_cache auxts "--test" "test_lru_cache")
add_test(test_multi_memtable auxts "--test" "test_multi_memtable")
add_test(test_extract auxts "--test" "test_extract")
add_test(test_hashtable auxts "--test" "test_hashtable")
add_test(test_murmurhash3 auxts "--test" "test_murmurhash3")
add_test(test_rfc3339 auxts "--test" "test_rfc3339")
add_test(test_simd_swap16 auxts "--test" "test_simd_swap16")
add_test(test_simd_swap24 auxts "--test" "test_simd_swap24")
add_test(test_simd_swap32 auxts "--test" "test_simd_swap32")
add_test(test_parser auxts "--test" "test_parser")
