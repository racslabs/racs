// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#include "dict.h"


int racs_dict_resize(racs_dict *dict, size_t new_capacity);

size_t racs_dict_index(racs_dict *dict, const void *key);

racs_dict_bucket *racs_dict_get_bucket(racs_dict *dict, const void *key);

int racs_dict_insert_entry(racs_dict *dict, racs_dict_bucket *bucket, void *key, void *value);

void racs_dict_remove_entry(racs_dict *dict, racs_dict_bucket *bucket, const void *key);

int racs_dict_validate_cb(const racs_dict *dict);


racs_dict *racs_dict_create(const size_t capacity, racs_dict_cb cb) {
    if (capacity == 0) {
        return NULL;
    }

    racs_dict *dict = malloc(sizeof(*dict));
    if (!dict) {
        return NULL;
    }

    dict->size = 0;
    dict->capacity = capacity;
    dict->cb = cb;

    dict->buckets = calloc(capacity, sizeof(racs_dict_bucket));
    if (!dict->buckets) {
        free(dict);
        return NULL;
    }

    return dict;
}

size_t racs_dict_index(racs_dict *dict, const void *key) {
    if (dict->capacity == 0) {
        return 0;
    }

    racs_dict_hash_cb hash = dict->cb.hash;
    return hash(key) % dict->capacity;
}

racs_dict_bucket *racs_dict_get_bucket(racs_dict *dict, const void *key) {
    if (!dict->buckets) {
        return NULL;
    }

    size_t index = racs_dict_index(dict, key);
    return &dict->buckets[index];
}

void *racs_dict_get(racs_dict *dict, const void *key) {
    if (!dict || !racs_dict_validate_cb(dict)) {
        return NULL;
    }

    racs_dict_bucket *bucket = racs_dict_get_bucket(dict, key);
    if (!bucket) {
        return NULL;
    }

    racs_dict_eq_cb eq = dict->cb.eq;
    for (racs_dict_entry *curr = bucket->head; curr; curr = curr->next) {
        if (eq(curr->key, key)) {
            return curr->value;
        }
    }

    return NULL;
}

void racs_dict_put(racs_dict *dict, const void *key, void *value) {
    if (!dict || dict->capacity == 0
        || !racs_dict_validate_cb(dict)) {
        return;
    }

    if (dict->size * 4 >= dict->capacity * 3) {
        if (!racs_dict_resize(dict, dict->capacity * 2)) {
            return;
        }
    }

    racs_dict_bucket *bucket = racs_dict_get_bucket(dict, key);
    if (!bucket) {
        return;
    }

    racs_dict_remove_entry(dict, bucket, key);
    racs_dict_insert_entry(dict, bucket, (void *) key, value);
}

void racs_dict_delete(racs_dict *dict, const void *key) {
    if (!dict || dict->capacity == 0
        || !racs_dict_validate_cb(dict)) {
        return;
    }

    racs_dict_bucket *bucket = racs_dict_get_bucket(dict, key);
    if (!bucket) {
        return;
    }

    racs_dict_remove_entry(dict, bucket, key);
}

void racs_dict_destroy(racs_dict *dict) {
    if (!dict) {
        return;
    }

    racs_dict_destroy_cb destroy = dict->cb.destroy;

    if (dict->buckets) {
        for (size_t i = 0; i < dict->capacity; i++) {
            racs_dict_entry *curr = dict->buckets[i].head;

            while (curr) {
                racs_dict_entry *next = curr->next;

                if (destroy) {
                    destroy(curr->key, curr->value);
                }

                free(curr);
                curr = next;
            }
        }

        free(dict->buckets);
    }

    free(dict);
}

void racs_dict_remove_entry(racs_dict *dict, racs_dict_bucket *bucket, const void *key) {
    racs_dict_entry *prev = NULL;
    racs_dict_entry *curr = bucket->head;

    racs_dict_eq_cb eq = dict->cb.eq;
    racs_dict_destroy_cb destroy = dict->cb.destroy;

    while (curr) {
        if (eq(curr->key, key)) {
            if (destroy) {
                destroy(curr->key, curr->value);
            }

            // remove node
            if (prev) {
                prev->next = curr->next;
            } else {
                bucket->head = curr->next;
            }

            free(curr);
            --bucket->count;
            --dict->size;

            return;
        }

        prev = curr;
        curr = curr->next;
    }
}

int racs_dict_insert_entry(racs_dict *dict, racs_dict_bucket *bucket, void *key, void *value) {
    if (!bucket) {
        return 0;
    }

    racs_dict_entry *entry = malloc(sizeof(*entry));
    if (!entry) {
        return 0;
    }

    entry->key = key;
    entry->value = value;
    entry->next = bucket->head;

    bucket->head = entry;
    ++bucket->count;
    ++dict->size;

    return 1;
}

int racs_dict_resize(racs_dict *dict, size_t new_capacity) {
    if (!dict->buckets) {
        return 0;
    }

    racs_dict_bucket *new_buckets = calloc(new_capacity, sizeof(*new_buckets));
    if (!new_buckets) {
        return 0;
    }

    for (size_t i = 0; i < dict->capacity; i++) {
        racs_dict_entry *curr = dict->buckets[i].head;

        while (curr) {
            racs_dict_entry *next = curr->next;

            racs_dict_hash_cb hash = dict->cb.hash;
            size_t index = hash(curr->key) % new_capacity;

            // insert at head of new bucket
            curr->next = new_buckets[index].head;
            new_buckets[index].head = curr;
            new_buckets[index].count++;

            curr = next;
        }
    }

    free(dict->buckets);

    dict->buckets = new_buckets;
    dict->capacity = new_capacity;

    return 1;
}

int racs_dict_validate_cb(const racs_dict *dict) {
    return dict->cb.hash &&
           dict->cb.eq &&
           dict->cb.destroy;
}
