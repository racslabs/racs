#include "hashmap.h"

static AUXTS__HashmapEntry* HashmapEntry_construct(const uint64_t* key, void* value);
static AUXTS__HashmapBucket* HashmapBucket_construct();
static void HashmapEntry_destroy(AUXTS__HashmapEntry* entry);

AUXTS__Hashmap* AUXTS__Hashmap_construct(size_t size) {
    AUXTS__Hashmap* map = malloc(sizeof(AUXTS__Hashmap));
    if (!map) {
        perror("Error allocating AUXTS__Hashmap");
        exit(EXIT_FAILURE);
    }

    map->size = size;

    map->buckets = malloc(size * sizeof(AUXTS__HashmapBucket*));
    if (!map->buckets) {
        perror("Failed to allocate buckets");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < map->size; ++i) {
        map->buckets[i] = HashmapBucket_construct();
    }

    return map;
}

AUXTS__HashmapBucket* HashmapBucket_construct() {
    AUXTS__HashmapBucket* bucket = malloc(sizeof(AUXTS__HashmapBucket));
    if (!bucket) {
        perror("Error allocating AUXTS__HashmapBucket");
        exit(EXIT_FAILURE);
    }

    bucket->count = 0;
    bucket->capacity = AUXTS_INITIAL_BUCKET_CAPACITY;

    bucket->entries = malloc(AUXTS_INITIAL_BUCKET_CAPACITY * sizeof(AUXTS__HashmapEntry*));
    if (!bucket->entries) {
        perror("Error allocating hashmap entries");
        exit(EXIT_FAILURE);
    }

    return bucket;
}


AUXTS__HashmapEntry* HashmapEntry_construct(const uint64_t* key, void* value) {
    AUXTS__HashmapEntry* entry = malloc(sizeof(AUXTS__HashmapEntry));
    if (!entry) {
        perror("Failed to allocate AUXTS__HashmapEntry");
        exit(EXIT_FAILURE);
    }

    entry->key[0] = key[0];
    entry->key[1] = key[1];
    entry->value = value;

    return entry;
}

void HashmapEntry_destroy(AUXTS__HashmapEntry* entry) {
    free(entry->value);
    free(entry);
}

void AUXTS__Hashmap_put(AUXTS__Hashmap* map, uint64_t* key, void* value) {
    uint64_t _key = AUXTS__hash((uint8_t *) key, 2 * sizeof(uint64_t), map->size);

    AUXTS__HashmapBucket* bucket = map->buckets[_key];

    for (int i = 0; i < bucket->count; ++i) {
        AUXTS__HashmapEntry* entry = bucket->entries[i];
        if (!entry) continue;

        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            free(entry->value);
            entry->value = value;
            return;
        }
    }

    AUXTS__HashmapEntry* entry = HashmapEntry_construct(key, value);

    if (bucket->count == bucket->capacity) {
        bucket->capacity = 1 << bucket->capacity;

        bucket->entries = realloc(bucket->entries, bucket->capacity * sizeof(AUXTS__HashmapEntry*));
        if (!bucket->entries) {
            perror("Error reallocating entries");
            exit(EXIT_FAILURE);
        }
    }

    bucket->entries[bucket->count] = entry;
    ++bucket->count;
}

void* AUXTS__Hashmap_get(AUXTS__Hashmap* map, uint64_t* key) {
    uint64_t _key = AUXTS__hash((uint8_t *) key, 2 * sizeof(uint64_t), map->size);

    AUXTS__HashmapBucket* bucket = map->buckets[_key];
    for (int i = 0; i < bucket->count; ++i) {
        AUXTS__HashmapEntry* entry = bucket->entries[i];
        if (!entry) continue;

        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            return entry->value;
        }
    }

    return NULL;
}

void AUXTS__Hashmap_delete(AUXTS__Hashmap* map, uint64_t* key) {
    uint64_t _key = AUXTS__hash((uint8_t *) key, 2 * sizeof(uint64_t), map->size);

    AUXTS__HashmapBucket* bucket = map->buckets[_key];
    for (int i = 0; i < bucket->count; ++i) {
        AUXTS__HashmapEntry* entry = bucket->entries[i];
        if (!entry) continue;

        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            HashmapEntry_destroy(entry);

            for (int j = i; j < bucket->count - 1; ++j) {
                bucket->entries[j] = bucket->entries[j + 1];
            }

            bucket->entries[bucket->count - 1] = NULL;
            bucket->count--;
            return;
        }
    }
}

void AUXTS__Hashmap_destroy(AUXTS__Hashmap* map) {
    for (int i = 0; i < map->size; ++i) {
        AUXTS__HashmapBucket* bucket = map->buckets[i];

        for (int j = 0; j < bucket->count; ++j) {
            AUXTS__HashmapEntry* entry = bucket->entries[j];
            if (!entry) continue;

            HashmapEntry_destroy(entry);
        }

        free(bucket->entries);
        free(bucket);
    }

    free(map->buckets);
    free(map);
}

int test_hashmap() {
    uint64_t key1[2];
    key1[0] = 1;
    key1[1] = 2;

    uint64_t key2[2];
    key2[0] = 3;
    key2[1] = 4;

    AUXTS__Hashmap* map = AUXTS__Hashmap_construct(3);

    uint8_t* data1 = (uint8_t*)strdup("data1");
    uint8_t* data2 = (uint8_t*)strdup("data2");
    uint8_t* data3 = (uint8_t*) strdup("data3");

    AUXTS__Hashmap_put(map, key1, data1);
    AUXTS__Hashmap_put(map, key2, data2);

    printf("%s\n", (char*) AUXTS__Hashmap_get(map, key1));
    printf("%s\n", (char*) AUXTS__Hashmap_get(map, key2));

    AUXTS__Hashmap_put(map, key1, data3);
    AUXTS__Hashmap_delete(map, key1);

    printf("%s\n", (char*) AUXTS__Hashmap_get(map, key1));

    AUXTS__Hashmap_destroy(map);

    return 0;
}
