#include "cache_test.h"

void test_lru_cache() {
    uint64_t key1[2] = {1, 2};
    uint64_t key2[2] = {3, 4};
    uint64_t key3[2] = {5, 6};

    uint8_t* data1 = (uint8_t*)strdup("data1");
    uint8_t* data2 = (uint8_t*)strdup("data2");
    uint8_t* data3 = (uint8_t*)strdup("data3");

    rats_cache* cache = rats_scache_create(2);
    rats_cache_put(cache, key1, data1);
    rats_cache_put(cache, key2, data2);
    rats_cache_put(cache, key3, data3);

    assert(strcmp("data3", (char*) rats_cache_get(cache, key3)) == 0);
    assert(strcmp("data2", (char*) rats_cache_get(cache, key2)) == 0);
    assert(rats_cache_get(cache, key1) == NULL);

    rats_cache_destroy(cache);
}
