// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#include "mt.h"
#include "mmh3.h"


racs_uint8 *racs_mt_to_sst(racs_mt * mt, size_t * sst_size);

racs_uint64 racs_mt_parts_hash_cb(const void *key);

int racs_mt_parts_eq_cb(const void *a, const void *b);

void racs_mt_parts_destroy_cb(void *key, void *value);

static racs_mmt *mmt_ = NULL;


void racs_mmt_init(void) {
    if (!mmt_) {
        racs_uint32 mmt_capacity = racs_config_get()->memtable.tables;
        racs_uint32 mt_capacity = racs_config_get()->memtable.entries;

        mmt_ = racs_mmt_create(mmt_capacity, mt_capacity);
    }
}

racs_mmt *racs_mmt_get(void) {
    if (!mmt_) {
        return NULL;
    }

    return mmt_;
}

void racs_mmt_iterator_init(racs_mmt_iter *iter, racs_mmt *mmt) {
    pthread_mutex_lock(&mmt->mutex);
    iter->mmt = mmt;
    iter->curr = mmt->head;
    pthread_mutex_unlock(&mmt->mutex);
}

racs_mt *racs_mmt_iterator_next(racs_mmt_iter *iter) {
    pthread_mutex_lock(&iter->mmt->mutex);

    racs_mt_node *prev = iter->curr;
    racs_mt_node *next = NULL;

    if (iter->curr == NULL) {
        next = iter->mmt->head;
    } else {
        next = iter->curr->next;
    }

    iter->curr = next;
    pthread_mutex_unlock(&iter->mmt->mutex);

    return next ? next->mt : NULL;
}

racs_mmt *racs_mmt_create(racs_uint32 mmt_capacity, racs_uint16 mt_capacity) {
    racs_mmt *mmt = malloc(sizeof(racs_mmt));
    if (!mmt) {
        return NULL;
    }

    pthread_mutex_init(&mmt->mutex, NULL);
    pthread_cond_init(&mmt->cond, NULL);

    mmt->mmt_capacity = mmt_capacity;
    mmt->mt_capacity = mt_capacity;
    mmt->num_tables = 0;

    mmt->head = NULL;
    mmt->tail = NULL;

    return mmt;
}

int racs_mmt_put(racs_mmt *mmt,
                 const racs_uint64 *key,
                 const racs_uint8 *block,
                 racs_uint16 block_size,
                 racs_uint32 checksum,
                 racs_uint64 lsn) {
    if (!mmt) {
        return -1;
    }

    pthread_mutex_lock(&mmt->mutex);
    // Ensure that we can append a new node to the head.
    // A new node is appended when head is null or current head is full.
    if (mmt->head == NULL || mmt->head->mt->num_entries >= mmt->mt_capacity) {
        // Check if mmt limit is reached
        if (mmt->mmt_capacity != 0 && mmt->num_tables >= mmt->mmt_capacity) {
            pthread_mutex_unlock(&mmt->mutex);
            return -1;
        }

        // Create new node and append to the head
        racs_mt_node *node = racs_mt_node_create(mmt->mt_capacity);
        if (!node) {
            pthread_mutex_unlock(&mmt->mutex);
            return -1;
        }

        racs_mmt_push_head(mmt, node);

        // Flush tail when 80% of the mmt capacity is reached
        if (mmt->num_tables > (mmt->mmt_capacity * 0.8)) {
            if (mmt->tail) {
                racs_mt_node *tail = racs_mmt_pop_tail(mmt);
                if (tail->mt) {
                    racs_mt_split_and_flush(tail->mt);
                    racs_mt_node_destroy(tail);
                }
            }
        }
    }

    racs_mt_put(mmt->head->mt, key, block, block_size, checksum, lsn);
    pthread_mutex_unlock(&mmt->mutex);

    return 0;
}

void racs_mmt_push_head(racs_mmt *mmt, racs_mt_node *node) {
    if (!mmt || !node) {
        return;
    }

    node->prev = NULL;
    node->next = mmt->head;

    if (mmt->head) {
        mmt->head->prev = node;
    } else {
        mmt->tail = node;
    }

    mmt->head = node;
    ++mmt->num_tables;
}

racs_mt_node *racs_mmt_pop_tail(racs_mmt *mmt) {
    if (!mmt || !mmt->tail) {
        return NULL;
    }

    racs_mt_node *node = mmt->tail;
    mmt->tail = node->prev;

    if (mmt->tail) {
        mmt->tail->next = NULL;
    } else {
        mmt->head = NULL;
    }

    node->prev = NULL;
    node->next = NULL;
    mmt->num_tables--;

    return node;
}

void racs_mmt_destroy(racs_mmt *mmt) {
    pthread_mutex_lock(&mmt->mutex);

    for (racs_mt_node *curr = mmt->head, *next; curr; curr = next) {
        next = curr->next;
        racs_mt_node_destroy(curr);
    }

    pthread_mutex_unlock(&mmt->mutex);
    pthread_mutex_destroy(&mmt->mutex);
}

racs_mt_node *racs_mt_node_create(racs_uint16 capacity) {
    racs_mt_node *node = malloc(sizeof(racs_mt_node));
    if (!node) {
        return NULL;
    }

    node->mt = racs_mt_create(capacity);
    if (!node->mt) {
        free(node);
        return NULL;
    }

    node->next = NULL;
    node->prev = NULL;

    return node;
}

void racs_mt_node_destroy(racs_mt_node *node) {
    if (!node) {
        return;
    }

    if (node->mt) {
        racs_mt_destroy(node->mt);
    }

    free(node);
}

racs_mt *racs_mt_create(racs_uint16 capacity) {
    racs_mt *mt = malloc(sizeof(racs_mt));
    if (!mt) {
        return NULL;
    }

    mt->num_entries = 0;
    mt->capacity = capacity;
    mt->entries = calloc(mt->capacity, sizeof(racs_mt_entry));
    if (!mt->entries) {
        free(mt);
        return NULL;
    }

    pthread_mutex_init(&mt->mutex, NULL);
    return mt;
}

void racs_mt_put(racs_mt *mt,
                 const racs_uint64 *key,
                 const racs_uint8 *block,
                 racs_uint16 block_size,
                 racs_uint32 checksum,
                 racs_uint64 lsn) {
    if (!mt) {
        return;
    }

    pthread_mutex_lock(&mt->mutex);

    if (mt->num_entries == mt->capacity) {
        pthread_mutex_unlock(&mt->mutex);
        return;
    }

    racs_mt_entry *entry = &mt->entries[mt->num_entries];

    entry->block = malloc(block_size);
    if (!entry->block) {
        pthread_mutex_unlock(&mt->mutex);
        return;
    }

    memcpy(entry->key, key, sizeof(racs_uint64) * 3);

    entry->lsn = lsn;
    entry->block_size = block_size;
    entry->checksum = checksum;

    memcpy(entry->block, block, block_size);
    ++mt->num_entries;

    pthread_mutex_unlock(&mt->mutex);
}

void racs_mt_flush(racs_mt *mt, const char *path) {
    if (!mt || mt->num_entries == 0) {
        return;
    }

    size_t sst_size = 0;
    racs_uint8 *sst = racs_mt_to_sst(mt, &sst_size);
    if (!sst) {
        return;
    }

    racs_queue *flush_q = racs_flush_queue_get();
    racs_flush_enqueue(flush_q, path, sst, sst_size);
}

void racs_mt_destroy(racs_mt *mt) {
    if (!mt) {
        return;
    }

    pthread_mutex_lock(&mt->mutex);

    if (mt->entries) {
        for (int i = 0; i < mt->num_entries; ++i) {
            if (mt->entries[i].block) {
                free(mt->entries[i].block);
            }
        }

        free(mt->entries);
    }

    pthread_mutex_unlock(&mt->mutex);
    pthread_mutex_destroy(&mt->mutex);

    free(mt);
}

racs_mt_parts *racs_mt_parts_create(racs_uint16 capacity) {
    racs_mt_parts *parts = malloc(sizeof(racs_mt_parts));
    if (!parts) {
        return NULL;
    }

    racs_dict_cb cb = {
        .hash = racs_mt_parts_hash_cb,
        .eq = racs_mt_parts_eq_cb,
        .destroy = racs_mt_parts_destroy_cb
    };

    parts->capacity = capacity;
    parts->dict = racs_dict_create(capacity, cb);
    if (!parts->dict) {
        free(parts);
        return NULL;
    }

    return parts;
}

void racs_mt_parts_put(racs_mt_parts *parts,
                       const racs_uint64 *key,
                       const racs_uint8 *block,
                       racs_uint16 block_size,
                       racs_uint32 checksum,
                       racs_uint64 lsn) {
    if (!parts || !parts->dict) {
        return;
    }

    racs_uint64 *part_key = malloc(2 * sizeof(racs_uint64));
    if (!part_key) {
        return;
    }

    part_key[0] = key[0]; // stream-id
    part_key[1] = key[2]; // version

    racs_mt *mt = racs_dict_get(parts->dict, part_key);
    if (!mt) {
        mt = racs_mt_create(parts->capacity);
        racs_dict_put(parts->dict, part_key, mt);
    } else {
        free(part_key);
    }

    racs_mt_put(mt, key, block, block_size, checksum, lsn);
}

void racs_mt_parts_destroy(racs_mt_parts *parts) {
    if (!parts) {
        return;
    }

    if (parts->dict) {
        racs_dict_destroy(parts->dict);
    }
}

void racs_mt_split_and_flush(racs_mt *mt) {
    if (!mt || mt->num_entries == 0) {
        return;
    }

    racs_mt_parts *parts = racs_mt_parts_create(mt->capacity);
    if (!parts) {
        return;
    }

    if (!parts->dict) {
        free(parts);
        return;
    }

    for (int i = 0; i < mt->num_entries; i++) {
        racs_mt_entry *entry = &mt->entries[i];

        //TODO: filter out old versions
        racs_mt_parts_put(parts, entry->key, entry->block,
                          entry->block_size, entry->checksum,
                          entry->lsn);
    }

    racs_dict *dict = parts->dict;
    for (int i = 0; i < dict->size; i++) {
        racs_dict_bucket *bucket = &dict->buckets[i];
        racs_dict_entry *curr = bucket->head;

        while (curr) {
            racs_dict_entry *next = curr->next;

            racs_mt *p_mt = (racs_mt *) curr->value;
            if (!p_mt || p_mt->num_entries == 0) {
                continue;
            }

            char path[PATH_MAX];

            racs_uint64 *key = p_mt->entries[0].key;
            racs_path_from_time(path, key[0], (racs_time) key[1]);
            racs_mt_flush(p_mt, path);

            curr = next;
        }
    }

    racs_mt_parts_destroy(parts);
}

racs_uint64 racs_mt_parts_hash_cb(const void *key) {
    racs_uint64 hash[2];
    racs_mmh3_x64_128(key, 2 * sizeof(racs_uint64), 0, hash);
    return hash[0];
}

int racs_mt_parts_eq_cb(const void *a, const void *b) {
    racs_uint64 *x = (racs_uint64 *) a;
    racs_uint64 *y = (racs_uint64 *) b;
    return x[0] == y[0] && x[1] == y[1];
}

void racs_mt_parts_destroy_cb(void *key, void *value) {
    free(key);
    racs_mt_destroy(value);
}

racs_uint8 *racs_mt_to_sst(racs_mt *mt, size_t *sst_size) {
    size_t data_size = 0;
    for (int i = 0; i < mt->num_entries; i++) {
        data_size += mt->entries[i].block_size;
    }

    size_t index_size = mt->num_entries * sizeof(racs_sst_index_entry);
    size_t trailer_size = sizeof(racs_uint16);
    size_t total_size = data_size + index_size + trailer_size;

    racs_uint8 *sst = malloc(total_size);
    if (!sst) {
        return NULL;
    }

    racs_uint8 *data_ptr = sst;
    racs_sst_index_entry *index_ptr = (racs_sst_index_entry *) (sst + data_size);

    for (int i = 0; i < mt->num_entries; i++) {
        racs_mt_entry *entry = &mt->entries[i];

        racs_uint32 offset = (racs_uint32)(data_ptr - sst);
        memcpy(data_ptr, entry->block, entry->block_size);

        data_ptr += entry->block_size;
        memcpy(index_ptr[i].key, entry->key, sizeof(racs_uint64) * 3);

        index_ptr[i].offset = offset;
        index_ptr[i].block_size = entry->block_size;
        index_ptr[i].checksum = entry->checksum;
    }

    memcpy(sst + (total_size - trailer_size), &mt->num_entries, trailer_size);
    *sst_size = total_size;

    return sst;
}
