#include "cache_test.h"


static racs_uint64 hash_callback(const void *key);

static int eq_callback(const void *a, const void *b);

static void destroy_callback(void *key, void *value);

static const racs_cache_callbacks CACHE_TEST_CALLBACKS = {
    .hash = hash_callback,
    .eq = eq_callback,
    .destroy = destroy_callback,
};

static int destroy_call_count = 0;


racs_uint64 hash_callback(const void *key) {
    racs_uint64 hash[2];
    racs_murmurhash3_x64_128(key, strlen((char *)key), 0, hash);
    return hash[0];
}

int eq_callback(const void *a, const void *b) {
    return strcmp((char *)a, (char *)b) == 0;
}

void destroy_callback(void *key, void *value) {
    destroy_call_count++;
    free(key);
    free(value);
}


void test_cache_get(void) {
    racs_cache *cache = racs_cache_create(2, CACHE_TEST_CALLBACKS);
    racs_cache_put(cache, strdup("a"), strdup("1"));
    racs_cache_put(cache, strdup("b"), strdup("2"));

    TEST_ASSERT_EQUAL_STRING("1", racs_cache_get(cache, "a"));
    TEST_ASSERT_EQUAL_STRING("2", racs_cache_get(cache, "b"));

    racs_cache_destroy(cache);
}

void test_cache_evict(void) {
    destroy_call_count = 0;

    racs_cache *cache = racs_cache_create(2, CACHE_TEST_CALLBACKS);
    racs_cache_put(cache, strdup("a"), strdup("1"));
    racs_cache_put(cache, strdup("b"), strdup("2"));
    racs_cache_put(cache, strdup("c"), strdup("3"));

    TEST_ASSERT_EQUAL_INT(1, destroy_call_count);
    TEST_ASSERT_NULL(racs_cache_get(cache, "a"));

    TEST_ASSERT_EQUAL_STRING("2", racs_cache_get(cache, "b"));
    TEST_ASSERT_EQUAL_STRING("3", racs_cache_get(cache, "c"));

    racs_cache_destroy(cache);
}