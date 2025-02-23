cmake_minimum_required(VERSION 3.27)
project(auxts C)

set(CMAKE_C_STANDARD 17)

enable_testing()

find_library(FLAC_LIBRARY NAMES FLAC)
find_path(FLAC_INCLUDE_DIR NAMES FLAC/all.h)

include_directories(${FLAC_INCLUDE_DIR})

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
        src/murmurhash3.h
        src/murmurhash3.c
        src/memtable.h
        src/memtable.c
        src/hashtable.h
        src/hashtable.c
        src/cache.h
        src/cache.c
        src/extract.h
        src/extract.c
        src/flac.h
        src/flac.c
        src/simd/simd_neon.h
        src/simd/simd.h
        src/simd/simd_neon-i32.c
        src/simd/simd_neon-i16.c
        test/test_runner.h
        test/cache_test.h
        test/cache_test.c
        test/extract_test.h
        test/extract_test.c
        test/memtable_test.h
        test/memtable_test.c
        test/hashtable_test.h
        test/hashtable_test.c
        test/murmurhash3_test.h
        test/murmurhash3_test.c
        test/timestamp_test.h
        test/timestamp_test.c
        test/simd_test.h
        test/simd_test.c
)

target_link_libraries(auxts ${FLAC_LIBRARY})

# Unit tests
add_test(test_lru_cache auxts "--test" "test_lru_cache")
add_test(test_multi_memtable auxts "--test" "test_multi_memtable")
add_test(test_extract auxts "--test" "test_extract")
add_test(test_hashtable auxts "--test" "test_hashtable")
add_test(test_murmurhash3 auxts "--test" "test_murmurhash3")
add_test(test_rfc3339 auxts "--test" "test_rfc3339")
add_test(test_simd_swap16 auxts "--test" "test_simd_swap16")
add_test(test_simd_swap24 auxts "--test" "test_simd_swap24")
add_test(test_simd_swap32, auxts "--test" "test_simd_swap32")
