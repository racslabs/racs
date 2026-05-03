#include "unity.h"
#include "dict_test.h"
#include "cache_test.h"


void setUp(void) {}

void tearDown(void) {}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_dict_get);
    RUN_TEST(test_dict_overwrite);
    RUN_TEST(test_dict_collision);
    RUN_TEST(test_cache_get);
    RUN_TEST(test_cache_evict);
    return UNITY_END();
}
