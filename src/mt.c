// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#include "mt.h"
#include "mmh3.h"


void racs_mt_node_rcu_cb(struct rcu_head *head);

void racs_mt_node_flush(racs_mt_node *node);

racs_uint8 *racs_mt_to_sst(racs_mt * mt, size_t * sst_size);

racs_uint64 racs_mt_parts_hash_cb(const void *key);

int racs_mt_parts_eq_cb(const void *a, const void *b);

void racs_mt_parts_destroy_cb(void *key, void *value);

int racs_mt_list_push_head(racs_mt_list *list, racs_mt_node *node);

void racs_mt_destroy_shallow(racs_mt *mt);

void *racs_mt_flush_worker(void *arg);

void racs_mt_flusher_signal(void);

racs_mt_flusher *racs_mt_flusher_create(racs_mt_list *list);

void racs_mt_flusher_destroy(racs_mt_flusher *flusher);

racs_mt_node *racs_mt_node_create(racs_uint16 capacity);

void racs_mt_node_destroy(racs_mt_node *node);

racs_mt *racs_mt_create(racs_uint16 capacity);

racs_mt_list *racs_mt_list_create(racs_uint16 capacity);


int racs_mt_put(racs_mt *mt,
                const racs_uint64 *key,
                const racs_uint8 *block,
                racs_uint16 block_size,
                racs_uint32 checksum,
                racs_uint64 lsn);

int racs_mt_flush(racs_mt *mt, const char *path);

void racs_mt_destroy(racs_mt *mt);

racs_mt_parts *racs_mt_parts_create(racs_uint16 capacity);

int racs_mt_parts_put(racs_mt_parts *parts,
                      const racs_uint64 *key,
                      const racs_uint8 *block,
                      racs_uint16 block_size,
                      racs_uint32 checksum,
                      racs_uint64 lsn);

void racs_mt_parts_destroy(racs_mt_parts *parts);

void racs_mt_split_and_flush(racs_mt *mt);


int racs_mt_put_local(racs_mt *mt, 
                      const racs_uint64 *key, 
                      const racs_uint8 *block, 
                      racs_uint16 block_size, 
                      racs_uint32 checksum, 
                      racs_uint64 lsn);


static racs_mt_list *list_ = NULL;       

static racs_mt_flusher *flusher_ = NULL;


void racs_mt_list_init(void) {
    rcu_register_thread();

    if (!list_) {
        racs_config *cfg = racs_config_get();
        if (!cfg) {
            exit(-1);
        }

        if (!list_) {
            list_ = racs_mt_list_create(cfg->memtable.entries);
            if (!list_) {
                exit(-1);
            }
        }
    }
}

racs_mt_list *racs_mt_list_get(void) {
    if (!list_) {
        return NULL;
    }

    return list_;
}

int racs_mt_flush_thread_start(void) {
    if (!list_) {
        return -1;
    }

    if (flusher_) {
        return -1;
    }

    flusher_ = racs_mt_flusher_create(list_);
    if (!flusher_) {
        return -1;
    }

    if (pthread_create(&flusher_->thread,
                       NULL,
                       racs_mt_flush_worker,
                       flusher_) != 0) {
        racs_mt_flusher_destroy(flusher_);
        flusher_ = NULL;
        return -1;
    }

    return 0;
}

racs_mt_flusher *racs_mt_flusher_create(racs_mt_list *list) {
    if (!list) {
        return NULL;
    }

    racs_mt_flusher *flusher = malloc(sizeof(*flusher));
    if (!flusher) {
        return NULL;
    }

    if (pthread_mutex_init(&flusher->mutex, NULL) != 0) {
        free(flusher);
        return NULL;
    }

    if (pthread_cond_init(&flusher->cond, NULL) != 0) {
        pthread_mutex_destroy(&flusher->mutex);
        free(flusher);
        return NULL;
    }

    racs_atomic_store(&flusher->running, true);
    racs_atomic_store(&flusher->flush_requested, false);

    flusher->list = list;

    return flusher;
}

void racs_mt_flusher_destroy(racs_mt_flusher *flusher) {
    if (!flusher) {
        return;
    }

    pthread_mutex_lock(&flusher->mutex);

    racs_atomic_store(&flusher->running, false);
    pthread_cond_signal(&flusher->cond);

    pthread_mutex_unlock(&flusher->mutex);

    pthread_join(flusher->thread, NULL);

    pthread_cond_destroy(&flusher->cond);
    pthread_mutex_destroy(&flusher->mutex);

    flusher->list = NULL;

    free(flusher);
}

void *racs_mt_flush_worker(void *arg) {
    racs_mt_flusher *flusher = (racs_mt_flusher *)arg;

    rcu_register_thread();

    for ( ; ; ) {
        pthread_mutex_lock(&flusher->mutex);

        while (!racs_atomic_load(&flusher->flush_requested) &&
               racs_atomic_load(&flusher->running)) {
            pthread_cond_wait(&flusher->cond, &flusher->mutex);
        }

        bool running = racs_atomic_load(&flusher->running);
        bool flush_requested = racs_atomic_load(&flusher->flush_requested);

        racs_atomic_store(&flusher->flush_requested, false);

        pthread_mutex_unlock(&flusher->mutex);

        if (!running) {
            break;
        }

        if (flush_requested) {
            /*
             * Rotation + flush goes here.
             */
        }
    }

    rcu_unregister_thread();

    return NULL;
}

void racs_mt_flusher_signal(void) {
    if (!flusher_) {
        return;
    }

    pthread_mutex_lock(&flusher_->mutex);

    racs_atomic_store(&flusher_->flush_requested, true);
    pthread_cond_signal(&flusher_->cond);

    pthread_mutex_unlock(&flusher_->mutex);
}

void racs_mt_list_iter(racs_mt_list *list, racs_mt_list_iter_cb cb, void *data) {
    if (!list || !cb) {
        return;
    }

    rcu_read_lock();

    racs_mt_node *curr = rcu_dereference(list->head);
    while (curr != NULL) {
        racs_mt *mt = curr->mt;
        if (mt) {
            racs_uint16 count = racs_atomic_load(&mt->num_entries);
            if (count > mt->capacity) {
                count = mt->capacity;
            }

            for (racs_uint16 i = 0; i < count; i++) {
                const racs_mt_entry *entry = &mt->entries[i];

                if (!racs_atomic_load(&entry->ready)) {
                    continue;
                }

                if (cb(entry, data) != 0) {
                    goto out;
                }
            }
        }

        curr = rcu_dereference(curr->next);
    }

out:
    rcu_read_unlock();
}

racs_mt_list *racs_mt_list_create(racs_uint16 capacity) {
    racs_mt_list *list = calloc(1, sizeof(racs_mt_list));
    if (!list) {
        return NULL;
    }

    list->capacity = capacity;

    racs_atomic_store(&list->size, 0);
    racs_atomic_store(&list->head, NULL);

    return list;
}


void racs_mt_list_destroy(racs_mt_list *list) {
    if (!list) {
        return;
    }

    racs_mt_node *curr = racs_atomic_load(&list->head);
    while (curr != NULL) {
        racs_mt_node *next = racs_atomic_load(&curr->next);
        racs_mt_node_destroy(curr);
        curr = next;
    }

    free(list);
}

int racs_mt_list_put(racs_mt_list *list,
                     const racs_uint64 *key,
                     const racs_uint8 *block,
                     racs_uint16 block_size,
                     racs_uint32 checksum,
                     racs_uint64 lsn) {
    if (!list || !key || !block || block_size == 0) {
        return -1;
    }

    racs_config *cfg = racs_config_get();
    if (!cfg) {
        return -1;
    }

    while (1) {
        racs_mt_node *curr_head = racs_atomic_load(&list->head);

        if (!curr_head) {
            racs_mt_node *new_node = racs_mt_node_create(cfg->memtable.entries);
            if (!new_node) {
                return -1;
            }

            if (racs_mt_list_push_head(list, new_node) != 0) {
                racs_mt_node_destroy(new_node);
                racs_mt_flusher_signal();

                continue;
            }

            continue; 
        }

        int ret = racs_mt_put(curr_head->mt, key, block, block_size, checksum, lsn);
        if (ret == 0) {
            return 0; 
        }

        if (ret == 1) {
            racs_mt_node *new_node = racs_mt_node_create(cfg->memtable.entries);
            if (!new_node) {
                return -1;
            }

            if (racs_mt_list_push_head(list, new_node) != 0) {
                racs_mt_node_destroy(new_node);
                racs_mt_flusher_signal();

                continue;
            }

            continue;
        }

        return -1;
    }
}

int racs_mt_put(racs_mt *mt,
                const racs_uint64 *key,
                const racs_uint8 *block,
                racs_uint16 block_size,
                racs_uint32 checksum,
                racs_uint64 lsn) {
    if (!mt || !key || !block || block_size == 0) {
        return -1;
    }

    if (racs_atomic_load(&mt->is_immutable)) {
        return 1;
    }

    racs_uint16 slot;
    do {
        slot = racs_atomic_load(&mt->num_entries);
        if (slot >= mt->capacity) {
            racs_atomic_store(&mt->is_immutable, true);
            return 1;
        }
    } while (!racs_atomic_cas(&mt->num_entries, &slot, slot + 1));

    racs_mt_entry *entry = &mt->entries[slot];
    
    entry->key[0] = key[0];
    entry->key[1] = key[1];
    entry->key[2] = key[2];
    entry->block_size = block_size;
    entry->checksum = checksum;
    entry->lsn = lsn;

    entry->block = malloc(block_size);
    if (!entry->block) {
        return -1;
    }

    memcpy(entry->block, block, block_size);

    racs_atomic_store(&entry->ready, true);

    if (slot + 1 == mt->capacity) {
        racs_atomic_store(&mt->is_immutable, true);
    }

    return 0;
}

int racs_mt_put_local(racs_mt *mt, 
                      const racs_uint64 *key, 
                      const racs_uint8 *block, 
                      racs_uint16 block_size, 
                      racs_uint32 checksum, 
                      racs_uint64 lsn) {
    if (!mt || !key || !block || mt->num_entries >= mt->capacity) {
        return -1;
    }

    racs_uint16 slot = mt->num_entries++;
    racs_mt_entry *entry = &mt->entries[slot];

    entry->key[0] = key[0];
    entry->key[1] = key[1];
    entry->key[2] = key[2];
    entry->checksum = checksum;
    entry->block_size = block_size;
    entry->lsn = lsn;
    
    entry->block = (racs_uint8 *)block; 
    entry->ready = true;

    return 0;
}

int racs_mt_list_push_head(racs_mt_list *list, racs_mt_node *node) {
    if (!list || !node) {
        return -1;
    }

    racs_uint16 size;

    do {
        size = racs_atomic_load(&list->size);

        if (size >= list->capacity) {
            return -1;
        }

    } while (!racs_atomic_cas(&list->size, &size, size + 1));

    racs_mt_node *old_head;

    do {
        old_head = racs_atomic_load(&list->head);
        racs_atomic_store(&node->next, old_head);
    } while (!racs_atomic_cas(&list->head, &old_head, node));

    return 0;
}

racs_mt_node *racs_mt_node_create(racs_uint16 capacity) {
    racs_mt *mt = racs_mt_create(capacity);
    if (!mt) {
        return NULL;
    }

    racs_mt_node *node = calloc(1, sizeof(racs_mt_node));
    if (!node) {
        return NULL;
    }

    node->mt = mt;
    racs_atomic_store(&node->next, NULL);

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
    if (capacity == 0) {
        return NULL;
    }    

    racs_mt *mt = calloc(1, sizeof(racs_mt));
    if (!mt) {
        return NULL;
    }

    mt->entries = calloc(capacity, sizeof(racs_mt_entry));
    if (!mt->entries) {
        free(mt);
        return NULL;
    }

    mt->capacity = capacity;
    racs_atomic_store(&mt->num_entries, 0);
    racs_atomic_store(&mt->is_immutable, false);

    return mt;
}

int racs_mt_flush(racs_mt *mt, const char *path) {
    if (!mt || mt->num_entries == 0) {
        return -1;
    }

    size_t sst_size = 0;
    racs_uint8 *sst = racs_mt_to_sst(mt, &sst_size);
    if (!sst) {
        return -1;
    }

    racs_fs_mkdir(path);
    int ret = racs_fs_write(path, sst, sst_size);

    free(sst);
    return ret;
}

void racs_mt_destroy(racs_mt *mt) {
    if (!mt) {
        return;
    }

    if (mt->entries) {
        racs_uint16 count = racs_atomic_load(&mt->num_entries);

        for (racs_uint16 i = 0; i < count; i++) {
            if (mt->entries[i].block) {
                free(mt->entries[i].block);
            }
        }

        free(mt->entries);
    }

    free(mt);
}

void racs_mt_destroy_shallow(racs_mt *mt) {
    if (!mt) {
        return;
    }
    
    if (mt->entries) {
        free(mt->entries);
    }

    free(mt);
}

void racs_mt_node_flush(racs_mt_node *node) {
    if (!node || !node->mt) {
        return;
    }

    racs_mt_split_and_flush(node->mt);
}

void racs_mt_node_rcu_cb(struct rcu_head *head) {
    racs_mt_node *node = caa_container_of(head, racs_mt_node, rcu);
    racs_mt_node_destroy(node);
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

int racs_mt_parts_put(racs_mt_parts *parts,
                      const racs_uint64 *key,
                      const racs_uint8 *block,
                      racs_uint16 block_size,
                      racs_uint32 checksum,
                      racs_uint64 lsn) {
    if (!parts || !parts->dict) {
        return -1;
    }

    racs_uint64 *part_key = malloc(2 * sizeof(racs_uint64));
    if (!part_key) {
        return -1;
    }

    part_key[0] = key[0]; // stream-id
    part_key[1] = key[2]; // version

    racs_mt *mt = racs_dict_get(parts->dict, part_key);
    if (!mt) {
        mt = racs_mt_create(parts->capacity);
        if (!mt) {
            free(part_key);
            return -1;
        }
        
        // TODO: handle failure
        racs_dict_put(parts->dict, part_key, mt);
    } else {
        free(part_key);
    }

    return racs_mt_put_local(mt, key, block, block_size, checksum, lsn);
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
    if (!mt) {
        return;
    }

    racs_uint16 count = racs_atomic_load(&mt->num_entries);
    if (count == 0) {
        return;
    }

    if (count > mt->capacity) {
        count = mt->capacity;
    }

    racs_mt_parts *parts = racs_mt_parts_create(mt->capacity);
    if (!parts) {
        return;
    }

    for (racs_uint16 i = 0; i < count; i++) {
        racs_mt_entry *entry = &mt->entries[i];

        while (!racs_atomic_load(&entry->ready)) {
            racs_cpu_pause();
        } 

        //TODO: filter out old versions
        //TODO: compress block here!
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
    racs_mt_destroy_shallow((racs_mt *) value);
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
