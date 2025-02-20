cmake_minimum_required(VERSION 3.27)
project(auxts C)

set(CMAKE_C_STANDARD 17)

enable_testing()

add_test(test_lru_cache auxts "--test" "test_lru_cache")
add_test(test_extract auxts "--test" "test_extract")
add_test(test_multi_memtable auxts "--test" "test_multi_memtable")

find_library(FLAC_LIBRARY NAMES FLAC)
find_path(FLAC_INCLUDE_DIR NAMES FLAC/all.h)

include_directories(${FLAC_INCLUDE_DIR})

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
        test/test_runner.h
        test/cache_test.h
        test/cache_test.c
        test/extract_test.h
        test/extract_test.c
        test/memtable_test.h
        test/memtable_test.c
)

target_link_libraries(auxts ${FLAC_LIBRARY})