#include "kvstore.h"

static void kvstore_bin_init(racs_kvstore_bin *bin);

static void kvstore_bin_append(racs_kvstore_bin *bin, void *key, void *value);

static racs_kvstore_bin *kvstore_get_bin(racs_kvstore *kv, void *key);

racs_kvstore *racs_kvstore_create(size_t capacity, racs_kvstore_hash_callback hash, racs_kvstore_cmp_callback cmp,
                                  racs_kvstore_destroy_callback destroy) {
    racs_kvstore *kv = malloc(sizeof(racs_kvstore));
    if (!kv) {
        perror("Error allocating racs_kvstore");
        return NULL;
    }

    kv->capacity = capacity;
    kv->ops.hash = hash;
    kv->ops.cmp = cmp;
    kv->ops.destroy = destroy;

    kv->bins = malloc(capacity * sizeof(racs_kvstore_bin));
    if (!kv->bins) {
        perror("Failed to allocate racs_kvstore bins");
        free(kv);
        return NULL;
    }

    for (int i = 0; i < capacity; ++i) {
        kvstore_bin_init(&kv->bins[i]);
    }

    return kv;
}

void kvstore_bin_init(racs_kvstore_bin *bin) {
    bin->count = 0;
    bin->capacity = 2;

    bin->entries = malloc(2 * sizeof(racs_kvstore_entry));
    if (!bin->entries) {
        perror("Error allocating racs_kvstore entries");
    }
}

void racs_kvstore_put(racs_kvstore *kv, void *key, void *value) {
    if (!kv) return;

    racs_kvstore_bin *bin = kvstore_get_bin(kv, key);

    for (int i = 0; i < bin->count; ++i) {
        racs_kvstore_entry *entry = &bin->entries[i];

        if (kv->ops.cmp(entry->key, key)) {
            kv->ops.destroy(entry->key, entry->value);
            entry->value = value;
            entry->key = key;
            return;
        }
    }

    kvstore_bin_append(bin, key, value);
}

void *racs_kvstore_get(racs_kvstore *kv, void *key) {
    if (!kv) return NULL;

    racs_kvstore_bin *bin = kvstore_get_bin(kv, key);

    for (int i = 0; i < bin->count; ++i) {
        racs_kvstore_entry *entry = &bin->entries[i];

        if (kv->ops.cmp(entry->key, key)) {
            return entry->value;
        }
    }

    return NULL;
}

void racs_kvstore_delete(racs_kvstore *kv, void *key) {
    if (!kv) return;

    racs_kvstore_bin *bin = kvstore_get_bin(kv, key);

    for (int i = 0; i < bin->count; ++i) {
        racs_kvstore_entry *entry = &bin->entries[i];

        if (kv->ops.cmp(entry->key, key)) {
            kv->ops.destroy(entry->key, entry->value);
            for (int j = i; j < bin->count - 1; ++j) {
                bin->entries[j] = bin->entries[j + 1];
            }

            --bin->count;
            return;
        }
    }
}

racs_kvstore_bin *kvstore_get_bin(racs_kvstore *kv, void *key) {
    racs_uint64 hash = kv->ops.hash(key) % kv->capacity;
    return &kv->bins[hash];
}

void racs_kvstore_destroy(racs_kvstore *kv) {
    for (int i = 0; i < kv->capacity; ++i) {
        racs_kvstore_bin *bin = &kv->bins[i];

        for (int j = 0; j < bin->count; ++j) {
            racs_kvstore_entry *entry = &bin->entries[j];
            kv->ops.destroy(entry->key, entry->value);
        }

        free(bin->entries);
    }

    free(kv->bins);
}

void kvstore_bin_append(racs_kvstore_bin *bin, void *key, void *value) {
    if (!bin) return;

    if (bin->count == bin->capacity) {
        bin->capacity *= bin->capacity;

        racs_kvstore_entry *entries = realloc(bin->entries, bin->capacity * sizeof(racs_kvstore_entry));
        if (!entries) {
            perror("Error reallocating entries");
            return;
        }

        bin->entries = entries;
    }

    bin->entries[bin->count].key = key;
    bin->entries[bin->count].value = value;

    ++bin->count;
}
