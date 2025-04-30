cmake_minimum_required(VERSION 3.27)
project(rats C)

set(CMAKE_C_STANDARD 17)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

find_package(Guile REQUIRED)
find_package(msgpack-c REQUIRED)
find_package(Lame)
find_package(Opus)
find_package(OpusEnc)

include_directories(
        ${OPUS_INCLUDE_DIRS}
        ${OPUSENC_INCLUDE_DIRS}
        ${LAME_INCLUDE_DIRS}
        ${GUILE_INCLUDE_DIRS}
        ${msgpack-c_INCLUDE_DIRS})

option(RATS_AVX2_CMAKE_BUILD "AVX2 CMake build" OFF)
option(RATS_NEON8_CMAKE_BUILD "NEON8 CMake build" ON)

if (RATS_AVX2_CMAKE_BUILD)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(-march=native -O2)
    endif()
elseif (RATS_NEON8_CMAKE_BUILD)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(ARM_ALIASES armv6l armv7l aarch64)
        add_compile_options(-O2 -march=armv8-a+simd)
    endif()
endif()

add_executable(rats
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
        test/wav_test.h
        test/wav_test.c
        test/format_test.h
        test/format_test.c
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
        test/stream_test.h
        test/stream_test.c
        src/conn.h
        src/conn.c
        src/server.h
        src/server.c
)

target_link_libraries(rats msgpack-c ${GUILE_LIBRARIES} ${LAME_LIBRARIES} ${OPUS_LIBRARIES} ${OPUSENC_LIBRARIES})

enable_testing()

# Unit test
add_test(test_metadata_attr rats "--test" "test_metadata_attr")
add_test(test_lru_cache rats "--test" "test_lru_cache")
add_test(test_multi_memtable rats "--test" "test_multi_memtable")
add_test(test_extract rats "--test" "test_extract")
add_test(test_extract_no_data rats "--test" "test_extract_no_data")
add_test(test_extract_error rats "--test" "test_extract_error")
add_test(test_extract_invalid_arg_type rats "--test" "test_extract_invalid_arg_type")
add_test(test_extract_invalid_num_args rats "--test" "test_extract_invalid_num_args")
add_test(test_command_executor_unknown_command rats "--test" "test_command_executor_unknown_command")
add_test(test_command_executor_parse_error1 rats "--test" "test_command_executor_parse_error1")
add_test(test_command_executor_is_not_command rats "--test" "test_command_executor_is_not_command")
add_test(test_hashtable rats "--test" "test_hashtable")
add_test(test_ping rats "--test" "test_ping")
add_test(test_murmurhash3 rats "--test" "test_murmurhash3")
add_test(test_rfc3339 rats "--test" "test_rfc3339")
add_test(test_parser rats "--test" "test_parser")
add_test(test_create rats "--test" "test_create")
add_test(test_scm_extract rats "--test" "test_scm_extract")
add_test(test_scm_create rats "--test" "test_scm_create")
add_test(test_scm_int rats "--test" "test_scm_int")
add_test(test_scm_list rats "--test" "test_scm_list")
add_test(test_scm_metadata rats "--test" "test_scm_metadata")
add_test(test_simd_interleave rats "--test" "test_simd_interleave")
add_test(test_write_wav rats "--test" "test_write_wav")
add_test(test_format_wav rats "--test" "test_format_wav")
add_test(test_streamopen rats "--test" "test_streamopen")
add_test(test_astp rats "--test" "test_astp")