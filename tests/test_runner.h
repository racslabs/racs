
#ifndef AUXTS_TEST_RUNNER_H
#define AUXTS_TEST_RUNNER_H

#include "../src/export.h"
#include "cache_test.h"
#include "extract_test.h"
#include "memtable_test.h"
#include "kvstore_test.h"
#include "murmur3_test.h"
#include "timestamp_test.h"
#include "simd_test.h"
#include "parser_test.h"
#include "command_executor_test.h"

typedef void (*test)();

AUXTS_FORCE_INLINE void auxts_add_test(test test_case, char* test_name, int argc, char* argv[]) {
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

AUXTS_FORCE_INLINE void auxts_run_tests(int argc, char* argv[]) {
    auxts_add_test(test_lru_cache, "test_lru_cache", argc, argv);
    auxts_add_test(test_extract, "test_extract", argc, argv);
    auxts_add_test(test_extract_error, "test_extract_error", argc, argv);
    auxts_add_test(test_extract_no_data, "test_extract_no_data", argc, argv);
    auxts_add_test(test_extract_invalid_num_args, "test_extract_invalid_num_args", argc, argv);
    auxts_add_test(test_extract_invalid_arg_type, "test_extract_invalid_arg_type", argc, argv);
    auxts_add_test(test_multi_memtable, "test_multi_memtable", argc, argv);
    auxts_add_test(test_hashtable, "test_hashtable", argc, argv);
    auxts_add_test(test_murmurhash3, "test_murmurhash3", argc, argv);
    auxts_add_test(test_rfc3339, "test_rfc3339", argc, argv);
    auxts_add_test(test_parser, "test_parser", argc, argv);
    auxts_add_test(test_command_executor_unknown_command, "test_command_executor_unknown_command", argc, argv);
    auxts_add_test(test_command_executor_parse_error1, "test_command_executor_parse_error1", argc, argv);
}

#endif //AUXTS_TEST_RUNNER_H
