#include "kvstore.h"

static void kvstore_bin_init(racs_kvstore_bin *bin);

static void kvstore_bin_append(racs_kvstore_bin *bin, void *key, void *value);

static racs_kvstore_bin *kvstore_get_bin(racs_kvstore *kv, void *key);

racs_kvstore *racs_kvstore_create(size_t capacity, racs_kvstore_hash_callback hash, racs_kvstore_cmp_callback cmp,
                                  racs_kvstore_destroy_callback destroy) {
    racs_kvstore *kv = malloc(sizeof(racs_kvstore));
    if (!kv) {
        racs_log_fatal("Error allocating racs_kvstore");
        return NULL;
    }

    kv->capacity = capacity;
    kv->ops.hash = hash;
    kv->ops.cmp = cmp;
    kv->ops.destroy = destroy;

    kv->bins = malloc(capacity * sizeof(racs_kvstore_bin));
    if (!kv->bins) {
        racs_log_fatal("Failed to allocate racs_kvstore bins");
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
    bin->node = NULL;
}

void racs_kvstore_put(racs_kvstore *kv, void *key, void *value) {
    if (!kv) return;

    racs_kvstore_bin *bin = kvstore_get_bin(kv, key);
    racs_kvstore_entry *curr = bin->node;

    while (curr) {
        if (kv->ops.cmp(curr->key, key)) {
            racs_kvstore_delete(kv, key);
            break;
        }

        curr = (racs_kvstore_entry *) curr->next;
    }

    kvstore_bin_append(bin, key, value);
}

void * racs_kvstore_get(racs_kvstore *kv, void *key) {
    if (!kv) return NULL;

    racs_kvstore_bin *bin = kvstore_get_bin(kv, key);
    racs_kvstore_entry *curr = bin->node;

    while (curr) {
        if (kv->ops.cmp(curr->key, key))
            return curr->value;

        curr = (racs_kvstore_entry *) curr->next;
    }

    return NULL;
}

void racs_kvstore_delete(racs_kvstore *kv, void *key) {
    if (!kv) return;

    racs_kvstore_bin *bin = kvstore_get_bin(kv, key);

    racs_kvstore_entry *prev = NULL;
    racs_kvstore_entry *curr = bin->node;

    while (curr) {
        if (kv->ops.cmp(curr->key, key)) {
            if (prev) prev->next = curr->next;
            else bin->node = (racs_kvstore_entry *) curr->next;

            kv->ops.destroy(curr->key, curr->value);
            free(curr);

            --bin->count;
            return;
        }

        prev = curr;
        curr = (racs_kvstore_entry *) curr->next;
    }
}

racs_kvstore_bin *kvstore_get_bin(racs_kvstore *kv, void *key) {
    racs_uint64 hash = kv->ops.hash(key) % kv->capacity;
    return &kv->bins[hash];
}

void racs_kvstore_destroy(racs_kvstore *kv) {
    for (int i = 0; i < kv->capacity; ++i) {
        racs_kvstore_bin *bin = &kv->bins[i];
        racs_kvstore_entry *curr = bin->node;

        while (curr) {
            racs_kvstore_entry *next = (racs_kvstore_entry *) curr->next;
            kv->ops.destroy(curr->key, curr->value);

            free(curr);
            curr = next;
        }
    }

    free(kv->bins);
}

void kvstore_bin_append(racs_kvstore_bin *bin, void *key, void *value) {
    if (!bin) return;

    racs_kvstore_entry *node = malloc(sizeof(racs_kvstore_entry));
    node->key = key;
    node->value = value;
    node->next = bin->node;

    bin->node = node;
    ++bin->count;
}
