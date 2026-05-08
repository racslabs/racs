#include "dict_test.h"


static racs_uint64 hash_cb(const void *key);

static racs_uint64 bad_hash_callback(const void *key);

static int eq_cb(const void *a, const void *b);

static void destroy_cb(void *key, void *value);

static const racs_dict_cb DICT_TEST_CALLBACKS = {
    .hash = hash_cb,
    .eq = eq_cb,
    .destroy = destroy_cb,
};


racs_uint64 hash_cb(const void *key) {
    racs_uint64 hash[2];
    racs_mmh3_x64_128(key, strlen((char *)key), 0, hash);
    return hash[0];
}

racs_uint64 bad_hash_callback(const void *key) {
    (void)key;
    return 1;
}

int eq_cb(const void *a, const void *b) {
    return strcmp((char *)a, (char *)b) == 0;
}

void destroy_cb(void *key, void *value) {
    free(key);
    free(value);
}


void test_dict_get(void) {
    racs_dict *dict = racs_dict_create(8, DICT_TEST_CALLBACKS);
    racs_dict_put(dict, strdup("a"), strdup("1"));
    racs_dict_put(dict, strdup("b"), strdup("2"));
    racs_dict_put(dict, strdup("c"), strdup("3"));

    TEST_ASSERT_EQUAL_STRING("1", racs_dict_get(dict, "a"));
    TEST_ASSERT_EQUAL_STRING("2", racs_dict_get(dict, "b"));
    TEST_ASSERT_EQUAL_STRING("3", racs_dict_get(dict, "c"));

    racs_dict_destroy(dict);
}

void test_dict_overwrite(void) {
    racs_dict *dict = racs_dict_create(8, DICT_TEST_CALLBACKS);
    racs_dict_put(dict, strdup("a"), strdup("1"));
    racs_dict_put(dict, strdup("a"), strdup("2"));

    TEST_ASSERT_EQUAL_STRING("2", racs_dict_get(dict, "a"));

    racs_dict_destroy(dict);
}

void test_dict_collision(void) {
    racs_dict_cb bad_callbacks = DICT_TEST_CALLBACKS;
    bad_callbacks.hash = bad_hash_callback;

    racs_dict *dict = racs_dict_create(8, bad_callbacks);
    racs_dict_put(dict, strdup("a"), strdup("1"));
    racs_dict_put(dict, strdup("b"), strdup("2"));
    racs_dict_put(dict, strdup("c"), strdup("3"));

    TEST_ASSERT_EQUAL_STRING("1", racs_dict_get(dict, "a"));
    TEST_ASSERT_EQUAL_STRING("2", racs_dict_get(dict, "b"));
    TEST_ASSERT_EQUAL_STRING("3", racs_dict_get(dict, "c"));

    racs_dict_destroy(dict);
}
