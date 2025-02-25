#include "hashtable_test.h"

void test_hashtable() {
    uint64_t key1[2] = {1, 2};
    uint64_t key2[2] = {3, 4};

    hashtable_t* hashtable = auxts_hashtable_create(3);

    uint8_t* data1 = (uint8_t*)strdup("data1");
    uint8_t* data2 = (uint8_t*)strdup("data2");
    uint8_t* data3 = (uint8_t*)strdup("data3");

    auxts_hashtable_put(hashtable, key1, data1);
    auxts_hashtable_put(hashtable, key2, data2);

    assert(strcmp("data1", (char*)auxts_hashtable_get(hashtable, key1)) == 0);
    assert(strcmp("data2", (char*)auxts_hashtable_get(hashtable, key2)) == 0);

    auxts_hashtable_put(hashtable, key1, data3);
    assert(strcmp("data3", (char*)auxts_hashtable_get(hashtable, key1)) == 0);

    auxts_hashtable_delete(hashtable, key1);
    assert(auxts_hashtable_get(hashtable, key1) == NULL);

    auxts_hashtable_destroy(hashtable);
}