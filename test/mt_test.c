#include "mt_test.h"


void test_mt_parts_put(void) {
    racs_uint64 key1[3] = {1, 1777838829, 0};
    racs_uint64 key2[3] = {2, 1777838831, 0};
    racs_uint64 key3[3] = {1, 1777838830, 0};
    racs_uint64 key4[3] = {2, 1777838832, 0};

    racs_mt_parts *parts = racs_mt_parts_create(4);
    racs_mt_parts_put(parts, key1, (racs_uint8 *)"1", strlen("1") + 1, 0, 0);
    racs_mt_parts_put(parts, key2, (racs_uint8 *)"a", strlen("a") + 1, 1, 1);
    racs_mt_parts_put(parts, key3, (racs_uint8 *)"2", strlen("2") + 1, 2, 2);
    racs_mt_parts_put(parts, key4, (racs_uint8 *)"b", strlen("b") + 1, 3, 3);

    racs_uint64 part_key1[2] = { 1, 0 };
    racs_uint64 part_key2[2] = { 2, 0 };

    racs_mt *mt1 = racs_dict_get(parts->dict, part_key1);
    racs_mt *mt2 = racs_dict_get(parts->dict, part_key2);

    TEST_ASSERT_EQUAL_UINT64(0ULL, mt1->entries[0].lsn);
    TEST_ASSERT_EQUAL_UINT64(2ULL, mt1->entries[1].lsn);
    TEST_ASSERT_EQUAL_UINT64(1ULL, mt2->entries[0].lsn);
    TEST_ASSERT_EQUAL_UINT64(3ULL, mt2->entries[1].lsn);
    TEST_ASSERT_EQUAL_UINT64(1ULL, mt1->entries[0].key[0]);
    TEST_ASSERT_EQUAL_UINT64(1777838829ULL, mt1->entries[0].key[1]);
    TEST_ASSERT_EQUAL_UINT64(0ULL, mt1->entries[0].key[2]);
    TEST_ASSERT_EQUAL_UINT64(1ULL, mt1->entries[1].key[0]);
    TEST_ASSERT_EQUAL_UINT64(1777838830ULL, mt1->entries[1].key[1]);
    TEST_ASSERT_EQUAL_UINT64(0ULL, mt1->entries[1].key[2]);
    TEST_ASSERT_EQUAL_UINT64(2ULL, mt2->entries[0].key[0]);
    TEST_ASSERT_EQUAL_UINT64(1777838831ULL, mt2->entries[0].key[1]);
    TEST_ASSERT_EQUAL_UINT64(0ULL, mt2->entries[0].key[2]);
    TEST_ASSERT_EQUAL_UINT64(2ULL, mt2->entries[1].key[0]);
    TEST_ASSERT_EQUAL_UINT64(1777838832ULL, mt2->entries[1].key[1]);
    TEST_ASSERT_EQUAL_UINT64(0ULL, mt2->entries[1].key[2]);
    TEST_ASSERT_EQUAL_STRING("1", mt1->entries[0].block);
    TEST_ASSERT_EQUAL_STRING("2", mt1->entries[1].block);
    TEST_ASSERT_EQUAL_STRING("a", mt2->entries[0].block);
    TEST_ASSERT_EQUAL_STRING("b", mt2->entries[1].block);

    racs_mt_parts_destroy(parts);
}

void test_mmt_put_sync(void) {
    racs_uint64 key1[3] = {1, 1777838829, 0};
    racs_uint64 key2[3] = {2, 1777838831, 0};
    racs_uint64 key3[3] = {1, 1777838830, 0};

    racs_mmt *mmt = racs_mmt_create(2, 1);

    TEST_ASSERT_EQUAL_INT(0, racs_mmt_put(mmt, key1, (racs_uint8 *)"1", strlen("1") + 1, 0, 0));
    TEST_ASSERT_EQUAL_INT(0, racs_mmt_put(mmt, key2, (racs_uint8 *)"a", strlen("a") + 1, 1, 1));
    TEST_ASSERT_EQUAL_INT(-1, racs_mmt_put(mmt, key3, (racs_uint8 *)"2", strlen("2") + 1, 2, 2));

    racs_mt_node *node = racs_mmt_pop_tail(mmt);
    TEST_ASSERT_EQUAL_INT(RACS_MT_STATE_FLUSHING, node->state);

    racs_mt *mt = node->mt;
    TEST_ASSERT_EQUAL_UINT64(0ULL, mt->entries[0].lsn);
    TEST_ASSERT_EQUAL_UINT64(1ULL, mt->entries[0].key[0]);
    TEST_ASSERT_EQUAL_UINT64(1777838829ULL, mt->entries[0].key[1]);
    TEST_ASSERT_EQUAL_UINT64(0ULL, mt->entries[0].key[2]);
    TEST_ASSERT_EQUAL_STRING("1", mt->entries[0].block);

    TEST_ASSERT_EQUAL_INT(0, racs_mmt_put(mmt, key3, (racs_uint8 *)"2", strlen("2") + 1, 2, 2));

    racs_mmt_destroy(mmt);
}