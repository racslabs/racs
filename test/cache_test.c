#include "cache_test.h"

void test_lru_cache() {
    uint64_t key1[2] = {1, 2};
    uint64_t key2[2] = {3, 4};
    uint64_t key3[2] = {5, 6};

    uint8_t* data1 = (uint8_t*)strdup("data1");
    uint8_t* data2 = (uint8_t*)strdup("data2");
    uint8_t* data3 = (uint8_t*)strdup("data3");

    racs_cache* cache = racs_scache_create(2);
    racs_cache_put(cache, key1, data1);
    racs_cache_put(cache, key2, data2);
    racs_cache_put(cache, key3, data3);

    assert(strcmp("data3", (char*) racs_cache_get(cache, key3)) == 0);
    assert(strcmp("data2", (char*) racs_cache_get(cache, key2)) == 0);
    assert(racs_cache_get(cache, key1) == NULL);

    racs_cache_destroy(cache);
}
