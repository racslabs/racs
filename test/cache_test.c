#include "cache_test.h"

void test_lru_cache() {
    uint64_t key1[2] = {1, 2};
    uint64_t key2[2] = {3, 4};
    uint64_t key3[2] = {5, 6};

    uint8_t* data1 = (uint8_t*)strdup("data1");
    uint8_t* data2 = (uint8_t*)strdup("data2");
    uint8_t* data3 = (uint8_t*)strdup("data3");

    cache_t* cache = auxts_cache_create(2);
    auxts_cache_put(cache, key1, data1);
    auxts_cache_put(cache, key2, data2);
    auxts_cache_put(cache, key3, data3);

    assert(strcmp("data3", (char*) auxts_cache_get(cache, key3)) == 0);
    assert(strcmp("data2", (char*) auxts_cache_get(cache, key2)) == 0);
    assert(auxts_cache_get(cache, key1) == NULL);

    auxts_cache_destroy(cache);
}
