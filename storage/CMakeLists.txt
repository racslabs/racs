cmake_minimum_required(VERSION 3.27)
project(auxts C)

set(CMAKE_C_STANDARD 17)

enable_testing()

add_test(rfc_test auxts "rfc")
add_test(murmur3_test auxts "murmur3")
add_test(memtable_test auxts "memtable")
add_test(list_test auxts "list")
add_test(hashmap_test auxts "hashmap")
add_test(cache_test auxts "cache")
add_test(extract_test auxts "extract")

add_executable(auxts main.c
        src/endian.h
        src/macro.h
        src/rfc.h
        src/rfc.c
        src/murmur3.h
        src/murmur3.c
        src/memtable.h
        src/memtable.c
        src/list.h
        src/list.c
        src/hashmap.h
        src/hashmap.c
        src/cache.h
        src/cache.c
        src/extract.h
        src/extract.c
)
