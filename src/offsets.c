
#include "offsets.h"


static racs_offsets *offsets = NULL;

static racs_uint64 racs_offsets_hash_cb(const void *key);

static int racs_offsets_eq_cb(const void *a, const void *b);

static void racs_offsets_destroy_cb(void *key, void *value);


void racs_offsets_init(void) {
    if (!offsets) {
        racs_dict_cb cb = {
            .hash = racs_offsets_hash_cb,
            .eq = racs_offsets_eq_cb,
            .destroy = racs_offsets_destroy_cb
        };

        racs_dict *dict = racs_dict_create(8, cb);
        if (!dict) {
            exit(-1);
        }

        offsets = malloc(sizeof(racs_offsets));
        if (!offsets) {
            racs_dict_destroy(dict);
            exit(-1);
        }

        pthread_mutex_init(&offsets->mutex, NULL);
        offsets->dict = dict;
    }
}

void racs_offsets_put(racs_offsets *offsets, racs_uint64 hash, racs_uint64 offset) {
    racs_uint64 *_hash = malloc(sizeof(racs_uint64));
    if (!_hash) {
        return;
    }

    racs_uint64 *_offset = malloc(sizeof(racs_uint64));
    if (!_offset) {
        free(_hash);
        return;
    }

    *_hash = hash;
    *_offset = offset;

    pthread_mutex_lock(&offsets->mutex);
    racs_dict_put(offsets->dict, _hash, _offset);
    pthread_mutex_unlock(&offsets->mutex);
}

racs_uint64 racs_offsets_get(racs_offsets *offsets, racs_uint64 hash) {
    pthread_mutex_lock(&offsets->mutex);
    racs_uint64 *offset = racs_dict_get(offsets->dict, &hash);
    pthread_mutex_unlock(&offsets->mutex);

    if (!offset) {
        return 0;
    }

    return *offset;
}

racs_uint64 racs_offsets_hash_cb(const void *key) {
    racs_uint64 hash[2];
    racs_mmh3_x64_128(key, sizeof(racs_uint64), 0, hash);
    return hash[0];
}

int racs_offsets_eq_cb(const void *a, const void *b) {
    racs_uint64 *x = (racs_uint64 *) a;
    racs_uint64 *y = (racs_uint64 *) b;
    return x[0] == y[0];
}

void racs_offsets_destroy_cb(void *key, void *value) {
    free(key);
    free(value);
}