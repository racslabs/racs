cmake_minimum_required(VERSION 3.27)
project(auxts C)

set(CMAKE_C_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Guile REQUIRED)
find_package(msgpack-c REQUIRED)
find_package(Lame)
find_package(Opus)

include_directories(
        ${OPUS_INCLUDE_DIRS}
        ${LAME_INCLUDE_DIRS}
        ${GUILE_INCLUDE_DIRS}
        ${msgpack-c_INCLUDE_DIRS})

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
        test/test_runner.h
        test/cache_test.h
        test/cache_test.c
        test/extract_test.h
        test/extract_test.c
        test/memtable_test.h
        test/memtable_test.c
        test/kvstore_test.h
        test/kvstore_test.c
        test/murmur3_test.h
        test/murmur3_test.c
        test/timestamp_test.h
        test/timestamp_test.c
        test/simd_test.h
        test/simd_test.c
        src/parser.h
        src/parser.c
        test/parser_test.h
        test/parser_test.c
        src/exec.h
        src/exec.c
        src/command.h
        src/command.c
        src/serialization.h
        src/serialization.c
        test/test_utils.h
        test/exec_test.h
        test/exec_test.c
        test/ping_test.h
        test/ping_test.c
        src/metadata.h
        src/metadata.c
        test/metadata_test.h
        test/metadata_test.c
        src/create.h
        src/create.c
        test/create_test.h
        test/create_test.c
        src/db.h
        src/db.c
        src/context.h
        src/context.c
        src/scm.h
        src/scm.c
        test/scm_test.h
        test/scm_test.c
        src/scm_bindings.h
        src/scm_bindings.c
        src/crc32c.h
        src/crc32c.c
        src/atsp.h
        src/atsp.c
        src/pcm.h
        src/types.h
        src/stream.h
        src/stream.c
        src/pcm.c
        src/simd/pcm_simd.h
        src/simd/pcm_neon.c
        src/extract.h
        src/extract.c
        src/wav.h
        src/wav.c
        test/wav_test.h
        test/wav_test.c
        test/format_test.h
        test/format_test.c
)

target_link_libraries(auxts msgpack-c ${GUILE_LIBRARIES} ${LAME_LIBRARIES} ${OPUS_LIBRARIES})

enable_testing()

# Unit test
add_test(test_metadata auxts "--test" "test_metadata")
add_test(test_metadata_attr auxts "--test" "test_metadata_attr")
add_test(test_lru_cache auxts "--test" "test_lru_cache")
add_test(test_multi_memtable auxts "--test" "test_multi_memtable")
add_test(test_extract auxts "--test" "test_extract")
add_test(test_extract_no_data auxts "--test" "test_extract_no_data")
add_test(test_extract_error auxts "--test" "test_extract_error")
add_test(test_extract_invalid_arg_type auxts "--test" "test_extract_invalid_arg_type")
add_test(test_extract_invalid_num_args auxts "--test" "test_extract_invalid_num_args")
add_test(test_command_executor_unknown_command auxts "--test" "test_command_executor_unknown_command")
add_test(test_command_executor_parse_error1 auxts "--test" "test_command_executor_parse_error1")
add_test(test_command_executor_is_not_command auxts "--test" "test_command_executor_is_not_command")
add_test(test_hashtable auxts "--test" "test_hashtable")
add_test(test_ping auxts "--test" "test_ping")
add_test(test_murmurhash3 auxts "--test" "test_murmurhash3")
add_test(test_rfc3339 auxts "--test" "test_rfc3339")
add_test(test_parser auxts "--test" "test_parser")
add_test(test_create auxts "--test" "test_create")
add_test(test_scm_extract auxts "--test" "test_scm_extract")
add_test(test_scm_create auxts "--test" "test_scm_create")
add_test(test_scm_int auxts "--test" "test_scm_int")
add_test(test_scm_list auxts "--test" "test_scm_list")
add_test(test_scm_metadata auxts "--test" "test_scm_metadata")
add_test(test_simd_interleave auxts "--test" "test_simd_interleave")
add_test(test_write_wav auxts "--test" "test_write_wav")
add_test(test_format_wav auxts "--test" "test_format_wav")
