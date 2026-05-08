#include "sst_test.h"


void test_sst_read(void) {
    racs_uint64 key1[3] = {1, 1777838829, 0};
    racs_uint64 key2[3] = {1, 1777838830, 1};

    racs_mt *mt = racs_mt_create(2);
    racs_mt_put(mt, key1, (racs_uint8 *)"hello", strlen("hello") + 1, 0, 0);
    racs_mt_put(mt, key2, (racs_uint8 *)"world", strlen("world") + 1, 1, 0);

    racs_mt_flush(mt, "test-sst");
    racs_mt_destroy(mt);

    racs_sst *sst = racs_sst_open("test-sst");
    racs_sst_index_entry *index = racs_sst_get_index(sst);

    char *str1 = (char *)(sst->data + index[0].offset);
    char *str2 = (char *)(sst->data + index[1].offset);

    TEST_ASSERT_EQUAL_STRING("hello", str1);
    TEST_ASSERT_EQUAL_STRING("world", str2);

    TEST_ASSERT_EQUAL_UINT32(0, index[0].checksum);
    TEST_ASSERT_EQUAL_UINT32(1, index[1].checksum);

    TEST_ASSERT_EQUAL_UINT64(key1[0], index[0].key[0]);
    TEST_ASSERT_EQUAL_UINT64(key2[0], index[1].key[0]);
    TEST_ASSERT_EQUAL_UINT64(key1[1], index[0].key[1]);
    TEST_ASSERT_EQUAL_UINT64(key2[1], index[1].key[1]);
    TEST_ASSERT_EQUAL_UINT64(key1[2], index[0].key[2]);
    TEST_ASSERT_EQUAL_UINT64(key2[2], index[1].key[2]);

    racs_sst_destroy(sst);
    unlink("test-sst");
}
