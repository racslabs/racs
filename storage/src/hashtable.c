#include "hashtable.h"

static HashtableEntry* hashtable_entry_create(const uint64_t* key, uint8_t* value);
static HashtableBucket* hashtable_bucket_create();
static void hashtable_bucket_append(HashtableBucket* bucket, HashtableEntry* entry);
static void hashtable_entry_destroy(HashtableEntry* entry);

Hashtable* auxts_hashtable_create(size_t num_entries) {
    Hashtable* hashtable = malloc(sizeof(Hashtable));
    if (!hashtable) {
        perror("Error allocating Hashtable");
        return NULL;
    }

    hashtable->num_buckets = num_entries;

    hashtable->buckets = malloc(num_entries * sizeof(HashtableBucket*));
    if (!hashtable->buckets) {
        perror("Failed to allocate buckets");
        free(hashtable);
        return NULL;
    }

    for (int i = 0; i < hashtable->num_buckets; ++i) {
        hashtable->buckets[i] = hashtable_bucket_create();
    }

    return hashtable;
}

void auxts_hashtable_put(Hashtable* hashtable, const uint64_t* key, uint8_t* value) {
    if (!hashtable) {
        return;
    }

    uint64_t _key = auxts_hash((uint8_t *) key, 2 * sizeof(uint64_t), hashtable->num_buckets);
    HashtableBucket* bucket = hashtable->buckets[_key];
    if (!bucket) {
        perror("HashtableBucket cannot be null");
        return;
    }

    for (int i = 0; i < bucket->count; ++i) {
        HashtableEntry* entry = bucket->entries[i];
        if (!entry) {
            continue;
        }

        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            free(entry->value);
            entry->value = value;
            return;
        }
    }

    HashtableEntry* entry = hashtable_entry_create(key, value);
    hashtable_bucket_append(bucket, entry);
}

void hashtable_bucket_append(HashtableBucket* bucket, HashtableEntry* entry) {
    if (!bucket || !entry) {
        return;
    }

    if (bucket->count == bucket->capacity) {
        bucket->capacity = 1 << bucket->capacity;

        HashtableEntry** entries = realloc(bucket->entries, bucket->capacity * sizeof(HashtableEntry*));
        if (!entries) {
            perror("Error reallocating entries");
            return;
        }

        bucket->entries = entries;
    }

    bucket->entries[bucket->count] = entry;
    ++bucket->count;
}

void* auxts_hashtable_get(Hashtable* hashtable, const uint64_t* key) {
    if (!hashtable) {
        return NULL;
    }

    uint64_t _key = auxts_hash((uint8_t *) key, 2 * sizeof(uint64_t), hashtable->num_buckets);
    HashtableBucket* bucket = hashtable->buckets[_key];
    if (!bucket) {
        perror("HashtableBucket cannot be null");
        return NULL;
    }

    for (int i = 0; i < bucket->count; ++i) {
        HashtableEntry* entry = bucket->entries[i];
        if (!entry) {
            continue;
        }

        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            return entry->value;
        }
    }

    return NULL;
}

void auxts_hashtable_delete(Hashtable* hashtable, uint64_t* key) {
    if (!hashtable) {
        return;
    }

    uint64_t _key = auxts_hash((uint8_t *) key, 2 * sizeof(uint64_t), hashtable->num_buckets);
    HashtableBucket* bucket = hashtable->buckets[_key];
    if (!bucket) {
        perror("HashtableBucket cannot be null");
        return;
    }

    for (int i = 0; i < bucket->count; ++i) {
        HashtableEntry* entry = bucket->entries[i];
        if (!entry) {
            continue;
        }

        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            hashtable_entry_destroy(entry);

            for (int j = i; j < bucket->count - 1; ++j) {
                bucket->entries[j] = bucket->entries[j + 1];
            }

            bucket->entries[bucket->count - 1] = NULL;
            --bucket->count;
            return;
        }
    }
}

void auxts_hashtable_destroy(Hashtable* hashtable) {
    for (int i = 0; i < hashtable->num_buckets; ++i) {
        HashtableBucket* bucket = hashtable->buckets[i];

        for (int j = 0; j < bucket->count; ++j) {
            HashtableEntry* entry = bucket->entries[j];
            hashtable_entry_destroy(entry);
        }

        free(bucket->entries);
        free(bucket);
    }

    free(hashtable->buckets);
    free(hashtable);
}

HashtableBucket* hashtable_bucket_create() {
    HashtableBucket* bucket = malloc(sizeof(HashtableBucket));
    if (!bucket) {
        perror("Error allocating HashtableBucket");
        return NULL;
    }

    bucket->count = 0;
    bucket->capacity = AUXTS_INITIAL_BUCKET_CAPACITY;

    bucket->entries = malloc(AUXTS_INITIAL_BUCKET_CAPACITY * sizeof(HashtableEntry*));
    if (!bucket->entries) {
        perror("Error allocating hashmap entries");
        free(bucket);
        return NULL;
    }

    return bucket;
}


HashtableEntry* hashtable_entry_create(const uint64_t* key, uint8_t* value) {
    HashtableEntry* entry = malloc(sizeof(HashtableEntry));
    if (!entry) {
        perror("Failed to allocate HashtableEntry");
        return NULL;
    }

    entry->key[0] = key[0];
    entry->key[1] = key[1];
    entry->value = value;

    return entry;
}

void hashtable_entry_destroy(HashtableEntry* entry) {
    free(entry->value);
    free(entry);
}

int test_hashtable() {
    uint64_t key1[2];
    key1[0] = 1;
    key1[1] = 2;

    uint64_t key2[2];
    key2[0] = 3;
    key2[1] = 4;

    Hashtable* map = auxts_hashtable_create(3);

    uint8_t* data1 = (uint8_t*)strdup("data1");
    uint8_t* data2 = (uint8_t*)strdup("data2");
    uint8_t* data3 = (uint8_t*) strdup("data3");

    auxts_hashtable_put(map, key1, data1);
    auxts_hashtable_put(map, key2, data2);

    printf("%s\n", (char*) auxts_hashtable_get(map, key1));
    printf("%s\n", (char*) auxts_hashtable_get(map, key2));

    auxts_hashtable_put(map, key1, data3);
    auxts_hashtable_delete(map, key1);

    printf("%s\n", (char*) auxts_hashtable_get(map, key1));

    auxts_hashtable_destroy(map);

    return 0;
}
