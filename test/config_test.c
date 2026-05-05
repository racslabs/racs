#include "config_test.h"


void test_config_load(void) {
    racs_config_load("test-conf.yaml");

    char *home = getenv("HOME");
    TEST_ASSERT_EQUAL_STRING(home, racs_config_get()->data_dir);
    TEST_ASSERT_EQUAL_STRING(home, racs_config_get()->log_dir);

    TEST_ASSERT_EQUAL_UINT32(6381, racs_config_get()->port);
    TEST_ASSERT_EQUAL_UINT32(4, racs_config_get()->memtable.tables);
    TEST_ASSERT_EQUAL_UINT32(2, racs_config_get()->memtable.entries);
    TEST_ASSERT_EQUAL_UINT32(2, racs_config_get()->cache.entries);
    TEST_ASSERT_EQUAL_UINT32(20, racs_config_get()->wal.fsync);

    racs_config_destroy();
}