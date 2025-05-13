
#ifndef RACS_TEST_RUNNER_H
#define RACS_TEST_RUNNER_H

#include "../src/export.h"
#include "cache_test.h"
#include "extract_test.h"
#include "memtable_test.h"
#include "kvstore_test.h"
#include "murmur3_test.h"
#include "timestamp_test.h"
#include "simd_test.h"
#include "parser_test.h"
#include "exec_test.h"
#include "ping_test.h"
#include "metadata_test.h"
#include "create_test.h"
#include "scm_test.h"
#include "wav_test.h"
#include "format_test.h"
#include "stream_test.h"

typedef void (*test)();

RACS_FORCE_INLINE void racs_add_test(test test_case, char* test_name, int argc, char* argv[]) {
    if (argc != 3) {
        return;
    }

    if (strcmp(argv[1], "--test") != 0) {
        return;
    }

    if (strcmp(test_name, argv[2]) == 0) {
        test_case();
    }
}

RACS_FORCE_INLINE void racs_run_tests(int argc, char* argv[]) {
    racs_add_test(test_lru_cache, "test_lru_cache", argc, argv);
    racs_add_test(test_extract, "test_extract", argc, argv);
    racs_add_test(test_extract_error, "test_extract_error", argc, argv);
    racs_add_test(test_extract_no_data, "test_extract_no_data", argc, argv);
    racs_add_test(test_extract_invalid_num_args, "test_extract_invalid_num_args", argc, argv);
    racs_add_test(test_extract_invalid_arg_type, "test_extract_invalid_arg_type", argc, argv);
    racs_add_test(test_multi_memtable, "test_multi_memtable", argc, argv);
    racs_add_test(test_hashtable, "test_hashtable", argc, argv);
    racs_add_test(test_murmurhash3, "test_murmurhash3", argc, argv);
    racs_add_test(test_rfc3339, "test_rfc3339", argc, argv);
    racs_add_test(test_parser, "test_parser", argc, argv);
    racs_add_test(test_ping, "test_ping", argc, argv);
    racs_add_test(test_command_executor_unknown_command, "test_command_executor_unknown_command", argc, argv);
    racs_add_test(test_command_executor_parse_error1, "test_command_executor_parse_error1", argc, argv);
    racs_add_test(test_command_executor_is_not_command, "test_command_executor_is_not_command", argc, argv);
    racs_add_test(test_metadata_attr, "test_metadata_attr", argc, argv);
    racs_add_test(test_create, "test_create", argc, argv);
    racs_add_test(test_scm_extract, "test_scm_extract", argc, argv);
    racs_add_test(test_scm_create, "test_scm_create", argc, argv);
    racs_add_test(test_scm_int, "test_scm_int", argc, argv);
    racs_add_test(test_scm_list, "test_scm_list", argc, argv);
    racs_add_test(test_scm_metadata, "test_scm_metadata", argc, argv);
    racs_add_test(test_simd_interleave, "test_simd_interleave", argc, argv);
    racs_add_test(test_write_wav, "test_write_wav", argc, argv);
    racs_add_test(test_format_wav, "test_format_wav", argc, argv);
    racs_add_test(test_streamopen, "test_streamopen", argc, argv);
    racs_add_test(test_atsp, "test_astp", argc, argv);
}

#endif //RACS_TEST_RUNNER_H
