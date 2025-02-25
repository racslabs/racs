#include "hashtable.h"

static hashtable_entry_t* hashtable_entry_create(const uint64_t* key, void* value);
static hashtable_bucket_t* hashtable_bucket_create();
static void hashtable_bucket_append(hashtable_bucket_t* bucket, hashtable_entry_t* entry);
static void hashtable_entry_destroy(hashtable_entry_t* entry);

hashtable_t* auxts_hashtable_create(size_t num_entries) {
    hashtable_t* ht = malloc(sizeof(hashtable_t));
    if (!ht) {
        perror("Error allocating ht");
        return NULL;
    }

    ht->num_buckets = num_entries;

    ht->buckets = malloc(num_entries * sizeof(hashtable_bucket_t*));
    if (!ht->buckets) {
        perror("Failed to allocate buckets");
        free(ht);
        return NULL;
    }

    for (int i = 0; i < ht->num_buckets; ++i) {
        ht->buckets[i] = hashtable_bucket_create();
    }

    return ht;
}

void auxts_hashtable_put(hashtable_t* ht, const uint64_t* key, void* value) {
    if (!ht) {
        return;
    }

    uint64_t _key = auxts_hash((uint8_t *) key, 2 * sizeof(uint64_t), ht->num_buckets);
    hashtable_bucket_t* bucket = ht->buckets[_key];
    if (!bucket) {
        perror("hashtable_bucket_t cannot be null");
        return;
    }

    for (int i = 0; i < bucket->count; ++i) {
        hashtable_entry_t* entry = bucket->entries[i];
        if (!entry) {
            continue;
        }

        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            free(entry->value);
            entry->value = value;
            return;
        }
    }

    hashtable_entry_t* entry = hashtable_entry_create(key, value);
    hashtable_bucket_append(bucket, entry);
}

void hashtable_bucket_append(hashtable_bucket_t* bucket, hashtable_entry_t* entry) {
    if (!bucket || !entry) {
        return;
    }

    if (bucket->count == bucket->capacity) {
        bucket->capacity = 1 << bucket->capacity;

        hashtable_entry_t** entries = realloc(bucket->entries, bucket->capacity * sizeof(hashtable_entry_t*));
        if (!entries) {
            perror("Error reallocating entries");
            return;
        }

        bucket->entries = entries;
    }

    bucket->entries[bucket->count] = entry;
    ++bucket->count;
}

void* auxts_hashtable_get(hashtable_t* ht, const uint64_t* key) {
    if (!ht) {
        return NULL;
    }

    uint64_t _key = auxts_hash((uint8_t *) key, 2 * sizeof(uint64_t), ht->num_buckets);
    hashtable_bucket_t* bucket = ht->buckets[_key];
    if (!bucket) {
        perror("hashtable_bucket_t cannot be null");
        return NULL;
    }

    for (int i = 0; i < bucket->count; ++i) {
        hashtable_entry_t* entry = bucket->entries[i];
        if (!entry) {
            continue;
        }

        if (entry->key[0] == key[0] && entry->key[1] == key[1]) {
            return entry->value;
        }
    }

    return NULL;
}

void auxts_hashtable_delete(hashtable_t* ht, uint64_t* key) {
    if (!ht) {
        return;
    }

    uint64_t _key = auxts_hash((uint8_t *) key, 2 * sizeof(uint64_t), ht->num_buckets);
    hashtable_bucket_t* bucket = ht->buckets[_key];
    if (!bucket) {
        perror("hashtable_bucket_t cannot be null");
        return;
    }

    for (int i = 0; i < bucket->count; ++i) {
        hashtable_entry_t* entry = bucket->entries[i];
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

void auxts_hashtable_destroy(hashtable_t* ht) {
    for (int i = 0; i < ht->num_buckets; ++i) {
        hashtable_bucket_t* bucket = ht->buckets[i];

        for (int j = 0; j < bucket->count; ++j) {
            hashtable_entry_t* entry = bucket->entries[j];
            hashtable_entry_destroy(entry);
        }

        free(bucket->entries);
        free(bucket);
    }

    free(ht->buckets);
    free(ht);
}

hashtable_bucket_t* hashtable_bucket_create() {
    hashtable_bucket_t* bucket = malloc(sizeof(hashtable_bucket_t));
    if (!bucket) {
        perror("Error allocating hashtable_bucket_t");
        return NULL;
    }

    bucket->count = 0;
    bucket->capacity = AUXTS_INITIAL_BUCKET_CAPACITY;

    bucket->entries = malloc(AUXTS_INITIAL_BUCKET_CAPACITY * sizeof(hashtable_entry_t*));
    if (!bucket->entries) {
        perror("Error allocating hashmap entries");
        free(bucket);
        return NULL;
    }

    return bucket;
}


hashtable_entry_t* hashtable_entry_create(const uint64_t* key, void* value) {
    hashtable_entry_t* entry = malloc(sizeof(hashtable_entry_t));
    if (!entry) {
        perror("Failed to allocate hashtable_entry_t");
        return NULL;
    }

    entry->key[0] = key[0];
    entry->key[1] = key[1];
    entry->value = value;

    return entry;
}

void hashtable_entry_destroy(hashtable_entry_t* entry) {
    free(entry->value);
    free(entry);
}
