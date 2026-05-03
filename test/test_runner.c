#include "unity.h"
#include "dict_test.h"


void setUp(void) {}

void tearDown(void) {}


int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_dict_get);
    RUN_TEST(test_dict_overwrite);
    RUN_TEST(test_dict_collision);
    return UNITY_END();
}
