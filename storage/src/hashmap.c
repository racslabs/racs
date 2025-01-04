#include "hashmap.h"

Hashmap* create_hashmap(size_t size) {
    Hashmap* map = malloc(sizeof(Hashmap));
    if (!map) {
        perror("Error allocating Hashmap");
        exit(-1);
    }

    map->size = size;

    map->buckets = malloc(size * sizeof(Bucket*));
    if (!map->buckets) {
        perror("Failed to allocate buckets");
        exit(-1);
    }

    for (int i = 0; i < map->size; ++i) {
        map->buckets[i] = create_bucket();
    }

    return map;
}

Bucket* create_bucket() {
    Bucket* bucket = malloc(sizeof(Bucket));
    if (!bucket) {
        perror("Error allocating Bucket");
        exit(-1);
    }

    bucket->count = 0;
    bucket->capacity = AUXTS_INITIAL_BUCKET_CAP;

    bucket->entries = malloc(AUXTS_INITIAL_BUCKET_CAP * sizeof(HashmapEntry*));
    if (!bucket->entries) {
        perror("Error allocating hashmap entries");
        exit(-1);
    }

    return bucket;
}


HashmapEntry* create_hashmap_entry(const uint64_t* key, void* value) {
    HashmapEntry* entry = malloc(sizeof(HashmapEntry));
    if (!entry) {
        perror("Failed to allocate HashmapEntry");
        exit(-1);
    }

    entry->key[0] = key[0];
    entry->key[1] = key[1];
    entry->value = value;

    return entry;
}

void destroy_hashmap_entry(HashmapEntry* entry) {
    free(entry->value);
    free(entry);
}

void put_hashmap(Hashmap* map, uint64_t* key, void* value) {
    uint64_t _key = hash((uint8_t*) key, 2 * sizeof(uint64_t), map->size);

    Bucket* bucket = map->buckets[_key];

    for (int i = 0; i < bucket->count; ++i) {
        HashmapEntry* entry = bucket->entries[i];
        if (!entry) continue;

        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            free(entry->value);
            entry->value = value;
            return;
        }
    }

    HashmapEntry* entry = create_hashmap_entry(key, value);

    if (bucket->count >= bucket->capacity) {
        bucket->capacity = 1 << bucket->capacity;

        bucket->entries = realloc(bucket->entries, bucket->capacity * sizeof(HashmapEntry*));
        if (!bucket->entries) {
            perror("Error reallocating entries");
            exit(-1);
        }
    }

    bucket->entries[bucket->count] = entry;
    ++bucket->count;
}

void* get_hashmap(Hashmap* map, uint64_t* key) {
    uint64_t _key = hash((uint8_t*) key, 2 * sizeof(uint64_t), map->size);

    Bucket* bucket = map->buckets[_key];
    for (int i = 0; i < bucket->count; ++i) {
        HashmapEntry* entry = bucket->entries[i];
        if (!entry) continue;

        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            return entry->value;
        }
    }

    return NULL;
}

void delete_hashmap(Hashmap* map, uint64_t* key) {
    uint64_t _key = hash((uint8_t*)key, 2 * sizeof(uint64_t), map->size);

    Bucket* bucket = map->buckets[_key];
    for (int i = 0; i < bucket->count; ++i) {
        HashmapEntry* entry = bucket->entries[i];
        if (!entry) continue;

        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            destroy_hashmap_entry(entry);

            for (int j = i; j < bucket->count - 1; ++j) {
                bucket->entries[j] = bucket->entries[j + 1];
            }

            bucket->entries[bucket->count - 1] = NULL;
            bucket->count--;
            return;
        }
    }
}

void destroy_hashmap(Hashmap* map) {
    for (int i = 0; i < map->size; ++i) {
        Bucket* bucket = map->buckets[i];

        for (int j = 0; j < bucket->count; ++j) {
            HashmapEntry* entry = bucket->entries[j];
            if (!entry) continue;

            destroy_hashmap_entry(entry);
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

    Hashmap* map = create_hashmap(3);

    uint8_t* data1 = (uint8_t*)strdup("data1");
    uint8_t* data2 = (uint8_t*)strdup("data2");
    uint8_t* data3 = (uint8_t*) strdup("data3");

    put_hashmap(map, key1, data1);
    put_hashmap(map, key2, data2);

    printf("%s\n", (char*)get_hashmap(map, key1));
    printf("%s\n", (char*)get_hashmap(map, key2));

    put_hashmap(map, key1, data3);

//    delete_hashmap(map, key1);

    printf("%s\n", (char*)get_hashmap(map, key1));

    destroy_hashmap(map);

    return 0;
}
