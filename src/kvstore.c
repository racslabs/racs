#include "kvstore.h"

static void kvstore_bin_init(rats_kvstore_bin *bin);

static void kvstore_bin_append(rats_kvstore_bin *bin, void *key, void *value);

static rats_kvstore_bin *kvstore_get_bin(rats_kvstore *kv, void *key);

rats_kvstore *rats_kvstore_create(size_t capacity, rats_kvstore_hash_callback hash, rats_kvstore_cmp_callback cmp,
                                  rats_kvstore_destroy_callback destroy) {
    rats_kvstore *kv = malloc(sizeof(rats_kvstore));
    if (!kv) {
        perror("Error allocating rats_kvstore");
        return NULL;
    }

    kv->capacity = capacity;
    kv->ops.hash = hash;
    kv->ops.cmp = cmp;
    kv->ops.destroy = destroy;

    kv->bins = malloc(capacity * sizeof(rats_kvstore_bin));
    if (!kv->bins) {
        perror("Failed to allocate rats_kvstore bins");
        free(kv);
        return NULL;
    }

    for (int i = 0; i < capacity; ++i) {
        kvstore_bin_init(&kv->bins[i]);
    }

    return kv;
}

void kvstore_bin_init(rats_kvstore_bin *bin) {
    bin->count = 0;
    bin->capacity = 2;

    bin->entries = malloc(2 * sizeof(rats_kvstore_entry));
    if (!bin->entries) {
        perror("Error allocating rats_kvstore entries");
    }
}

void rats_kvstore_put(rats_kvstore *kv, void *key, void *value) {
    if (!kv) return;

    rats_kvstore_bin *bin = kvstore_get_bin(kv, key);

    for (int i = 0; i < bin->count; ++i) {
        rats_kvstore_entry *entry = &bin->entries[i];

        if (kv->ops.cmp(entry->key, key)) {
            kv->ops.destroy(entry->key, entry->value);
            entry->value = value;
            entry->key = key;
            return;
        }
    }

    kvstore_bin_append(bin, key, value);
}

void *rats_kvstore_get(rats_kvstore *kv, void *key) {
    if (!kv) return NULL;

    rats_kvstore_bin *bin = kvstore_get_bin(kv, key);

    for (int i = 0; i < bin->count; ++i) {
        rats_kvstore_entry *entry = &bin->entries[i];

        if (kv->ops.cmp(entry->key, key)) {
            return entry->value;
        }
    }

    return NULL;
}

void rats_kvstore_delete(rats_kvstore *kv, void *key) {
    if (!kv) return;

    rats_kvstore_bin *bin = kvstore_get_bin(kv, key);

    for (int i = 0; i < bin->count; ++i) {
        rats_kvstore_entry *entry = &bin->entries[i];

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

rats_kvstore_bin *kvstore_get_bin(rats_kvstore *kv, void *key) {
    rats_uint64 hash = kv->ops.hash(key) % kv->capacity;
    return &kv->bins[hash];
}

void rats_kvstore_destroy(rats_kvstore *kv) {
    for (int i = 0; i < kv->capacity; ++i) {
        rats_kvstore_bin *bin = &kv->bins[i];

        for (int j = 0; j < bin->count; ++j) {
            rats_kvstore_entry *entry = &bin->entries[j];
            kv->ops.destroy(entry->key, entry->value);
        }

        free(bin->entries);
    }

    free(kv->bins);
}

void kvstore_bin_append(rats_kvstore_bin *bin, void *key, void *value) {
    if (!bin) return;

    if (bin->count == bin->capacity) {
        bin->capacity *= bin->capacity;

        rats_kvstore_entry *entries = realloc(bin->entries, bin->capacity * sizeof(rats_kvstore_entry));
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
