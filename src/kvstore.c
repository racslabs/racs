#include "kvstore.h"

static void kvstore_bin_init(auxts_kvstore_bin* bin);
static void kvstore_bin_append(auxts_kvstore_bin* bin, void* key, void* value);
static auxts_kvstore_bin* kvstore_get_bin(auxts_kvstore* kv, void* key);

auxts_kvstore* auxts_kvstore_create(size_t capacity, auxts_kvstore_hash_callback hash, auxts_kvstore_cmp_callback cmp, auxts_kvstore_destroy_callback destroy) {
    auxts_kvstore* kv = malloc(sizeof(auxts_kvstore));
    if (!kv) {
        perror("Error allocating auxts_kvstore");
        return NULL;
    }

    kv->capacity = capacity;
    kv->ops.hash = hash;
    kv->ops.cmp = cmp;
    kv->ops.destroy = destroy;

    kv->bins = malloc(capacity * sizeof(auxts_kvstore_bin));
    if (!kv->bins) {
        perror("Failed to allocate auxts_kvstore bins");
        free(kv);
        return NULL;
    }

    for (int i = 0; i < capacity; ++i) {
        kvstore_bin_init(&kv->bins[i]);
    }

    return kv;
}

void kvstore_bin_init(auxts_kvstore_bin* bin) {
    bin->count = 0;
    bin->capacity = 2;

    bin->entries = malloc(2 * sizeof(auxts_kvstore_entry));
    if (!bin->entries) {
        perror("Error allocating auxts_kvstore entries");
    }
}

void auxts_kvstore_put(auxts_kvstore* kv, void* key, void* value) {
    if (!kv) return;

    auxts_kvstore_bin* bin = kvstore_get_bin(kv, key);

    for (int i = 0; i < bin->count; ++i) {
        auxts_kvstore_entry* entry = &bin->entries[i];

        if (kv->ops.cmp(entry->key, key)) {
            kv->ops.destroy(entry->key, entry->value);
            entry->value = value;
            return;
        }
    }

    kvstore_bin_append(bin, key, value);
}

void* auxts_kvstore_get(auxts_kvstore* kv, void* key) {
    if (!kv) return NULL;

    auxts_kvstore_bin* bin = kvstore_get_bin(kv, key);

    for (int i = 0; i < bin->count; ++i) {
        auxts_kvstore_entry* entry = &bin->entries[i];

        if (kv->ops.cmp(entry->key, key)) {
            return entry->value;
        }
    }

    return NULL;
}

void auxts_kvstore_delete(auxts_kvstore* kv, void* key) {
    if (!kv) return;

    auxts_kvstore_bin* bin = kvstore_get_bin(kv, key);

    for (int i = 0; i < bin->count; ++i) {
        auxts_kvstore_entry* entry = &bin->entries[i];

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

auxts_kvstore_bin* kvstore_get_bin(auxts_kvstore* kv, void* key) {
    uint64_t hash = kv->ops.hash(key) % kv->capacity;
    return &kv->bins[hash];
}

void auxts_kvstore_destroy(auxts_kvstore* kv) {
    for (int i = 0; i < kv->capacity; ++i) {
        auxts_kvstore_bin* bin = &kv->bins[i];

        for (int j = 0; j < bin->count; ++j) {
            auxts_kvstore_entry* entry = &bin->entries[j];
            kv->ops.destroy(entry->key, entry->value);
        }

        free(bin->entries);
    }

    free(kv->bins);
}

void kvstore_bin_append(auxts_kvstore_bin* bin, void* key, void* value) {
    if (!bin) return;

    if (bin->count == bin->capacity) {
        bin->capacity *= bin->capacity;

        auxts_kvstore_entry* entries = realloc(bin->entries, bin->capacity * sizeof(auxts_kvstore_entry));
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
