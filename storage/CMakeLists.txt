cmake_minimum_required(VERSION 3.27)
project(auxts C)

set(CMAKE_C_STANDARD 17)

enable_testing()

add_test(rfc_test auxts "--test" "test_lru_cache")
#add_test(murmur3_test auxts "--test" "murmurhash3")
#add_test(memtable_test auxts "--test" "memtable")
#add_test(hashmap_test auxts "--test" "hashtable")
#add_test(cache_test auxts "--test" "cache")
#add_test(extract_test auxts "--test" "extract")

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
)

target_link_libraries(auxts ${FLAC_LIBRARY})