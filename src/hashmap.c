#include "hashmap.h"

static void hashmap_bucket_init(auxts_hashmap_bucket* bucket);
static void hashmap_bucket_append(auxts_hashmap_bucket* bucket, void* key, void* value);
static auxts_hashmap_bucket* hashmap_get_bucket(auxts_hashmap* map, void* key);

auxts_hashmap* auxts_hashmap_create(size_t capacity, auxts_hash_func hash, auxts_cmp_func cmp, auxts_destroy_func destroy) {
    auxts_hashmap* map = malloc(sizeof(auxts_hashmap));
    if (!map) {
        perror("Error allocating auxts_hashmap");
        return NULL;
    }

    map->num_buckets = capacity;
    map->ops.hash = hash;
    map->ops.cmp = cmp;
    map->ops.destroy = destroy;

    map->buckets = malloc(capacity * sizeof(auxts_hashmap_bucket));
    if (!map->buckets) {
        perror("Failed to allocate auxts_hashmap buckets");
        free(map);
        return NULL;
    }

    for (int i = 0; i < capacity; ++i) {
        hashmap_bucket_init(&map->buckets[i]);
    }

    return map;
}

void hashmap_bucket_init(auxts_hashmap_bucket* bucket) {
    bucket->count = 0;
    bucket->capacity = 2;

    bucket->entries = malloc(2 * sizeof(auxts_hashmap_entry));
    if (!bucket->entries) {
        perror("Error allocating auxts_hashmap entries");
    }
}

void auxts_hashmap_put(auxts_hashmap* map, void* key, void* value) {
    if (!map) return;

    auxts_hashmap_bucket* bucket = hashmap_get_bucket(map, key);

    for (int i = 0; i < bucket->count; ++i) {
        auxts_hashmap_entry* entry = &bucket->entries[i];

        if (map->ops.cmp(entry->key, key)) {
            free(entry->value);
            entry->value = value;
            return;
        }
    }

    hashmap_bucket_append(bucket, key, value);
}

void* auxts_hashmap_get(auxts_hashmap* map, void* key) {
    if (!map) return NULL;

    auxts_hashmap_bucket* bucket = hashmap_get_bucket(map, key);

    for (int i = 0; i < bucket->count; ++i) {
        auxts_hashmap_entry* entry = &bucket->entries[i];

        if (map->ops.cmp(entry->key, key)) {
            return entry->value;
        }
    }

    return NULL;
}

void auxts_hashmap_delete(auxts_hashmap* map, void* key) {
    if (!map) return;

    auxts_hashmap_bucket* bucket = hashmap_get_bucket(map, key);

    for (int i = 0; i < bucket->count; ++i) {
        auxts_hashmap_entry* entry = &bucket->entries[i];

        if (map->ops.cmp(entry->key, key)) {
            map->ops.destroy(entry->key, entry->value);

            for (int j = i; j < bucket->count - 1; ++j) {
                bucket->entries[j] = bucket->entries[j + 1];
            }

            --bucket->count;
            return;
        }
    }
}

auxts_hashmap_bucket* hashmap_get_bucket(auxts_hashmap* map, void* key) {
    uint64_t hash = map->ops.hash(key) % map->num_buckets;
    return &map->buckets[hash];
}

void auxts_hashmap_destroy(auxts_hashmap* map) {
    for (int i = 0; i < map->num_buckets; ++i) {
        auxts_hashmap_bucket* bucket = &map->buckets[i];

        for (int j = 0; j < bucket->count; ++j) {
            auxts_hashmap_entry* entry = &bucket->entries[j];
            map->ops.destroy(entry->key, entry->value);
        }

        free(bucket->entries);
    }

    free(map->buckets);
}

void hashmap_bucket_append(auxts_hashmap_bucket* bucket, void* key, void* value) {
    if (!bucket) return;

    if (bucket->count == bucket->capacity) {
        bucket->capacity = 1 << bucket->capacity;

        auxts_hashmap_entry* entries = realloc(bucket->entries, bucket->capacity * sizeof(auxts_hashmap_entry));
        if (!entries) {
            perror("Error reallocating entries");
            return;
        }

        bucket->entries = entries;
    }

    bucket->entries[bucket->count].key = key;
    bucket->entries[bucket->count].value = value;

    ++bucket->count;
}
