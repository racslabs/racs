#include "unity.h"
#include "dict_test.h"
#include "cache_test.h"
#include "sstable_test.h"
#include "time_test.h"


void setUp(void) {}

void tearDown(void) {}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_dict_get);
    RUN_TEST(test_dict_overwrite);
    RUN_TEST(test_dict_collision);
    RUN_TEST(test_cache_get);
    RUN_TEST(test_cache_evict);
    RUN_TEST(test_sstable_read);
    RUN_TEST(test_time_from_ts);
    RUN_TEST(test_time_to_tm);
    RUN_TEST(test_time_to_rfc3339);
    RUN_TEST(test_time_from_path);
    return UNITY_END();
}
