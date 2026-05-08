#include "unity.h"
#include "dict_test.h"
#include "cache_test.h"
#include "config_test.h"
#include "mt_test.h"
#include "sst_test.h"
#include "time_test.h"
#include "path_test.h"


void setUp(void) {}
void tearDown(void) {}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_dict_get);
    RUN_TEST(test_dict_overwrite);
    RUN_TEST(test_dict_collision);
    RUN_TEST(test_cache_get);
    RUN_TEST(test_cache_evict);
    RUN_TEST(test_sst_read);
    RUN_TEST(test_time_from_ts);
    RUN_TEST(test_time_to_tm);
    RUN_TEST(test_time_to_rfc3339);
    RUN_TEST(test_time_from_path);
    RUN_TEST(test_config_load);
    RUN_TEST(test_path_expand);
    RUN_TEST(test_path_from_time);
    RUN_TEST(test_path_from_range);
    RUN_TEST(test_path_resolve);
    RUN_TEST(test_mt_parts_put);
    return UNITY_END();
}
