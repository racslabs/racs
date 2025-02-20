
#ifndef AUXTS_TEST_RUNNER_H
#define AUXTS_TEST_RUNNER_H

#include "../src/export.h"
#include "cache_test.h"

typedef void (*Test)();

AUXTS_FORCE_INLINE void auxts_add_test(Test test_case, char* test_name, int argc, char* argv[]) {
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
}

#endif //AUXTS_TEST_RUNNER_H
