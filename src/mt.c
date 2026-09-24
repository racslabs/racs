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

racs_mt_node *racs_mt_list_pop_tail(racs_mt_list *list);

int racs_mt_list_push_head(racs_mt_list *list, racs_mt_node *node);

void racs_mt_destroy_shallow(racs_mt *mt);

void racs_mt_node_queue_cb(void *data);

void *racs_mt_flush_worker(void *arg);

racs_mt_node *racs_mt_node_create(racs_uint16 capacity);

void racs_mt_node_destroy(racs_mt_node *node);

racs_mt *racs_mt_create(racs_uint16 capacity);

racs_mt_list *racs_mt_list_create(racs_uint16 capacity);

void racs_mt_node_queue_init(void);

racs_queue *racs_mt_node_queue_get(void);

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

static racs_queue *queue_ = NULL;


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

void racs_mt_node_queue_init(void) {
    if (!queue_) {
        queue_ = racs_queue_create(racs_mt_node_queue_cb);
        if (!queue_) {
            exit(-1);
        }
    }
}

racs_queue *racs_mt_node_queue_get(void) {
    if (!queue_) {
        return NULL;
    }

    return queue_;
}

void racs_mt_flush_thread_start(void) {
    racs_mt_node_queue_init();

    pthread_t thread;
    pthread_create(&thread, NULL, racs_mt_flush_worker, NULL);
    pthread_detach(thread);
}

void *racs_mt_flush_worker(void *arg) {
    (void) arg;

    rcu_register_thread();

    for ( ; ; ) {
        racs_mt_node *node = (racs_mt_node *) racs_dequeue(queue_);
        if (!node) {
            break;
        }

        racs_mt_node_flush(node);

        call_rcu(&node->rcu, racs_mt_node_rcu_cb);
    }

    rcu_unregister_thread();
    return NULL;
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
            racs_uint16 count = atomic_load_explicit(&mt->num_entries, memory_order_acquire);
            if (count > mt->capacity) {
                count = mt->capacity;
            }

            for (racs_uint16 i = 0; i < count; i++) {
                const racs_mt_entry *entry = &mt->entries[i];

                if (!atomic_load_explicit(&entry->ready, memory_order_acquire)) {
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

    atomic_store_explicit(&list->size, 0, memory_order_relaxed);
    atomic_store_explicit(&list->head, NULL, memory_order_relaxed);

    pthread_mutex_init(&list->mutex, NULL);

    return list;
}


void racs_mt_list_destroy(racs_mt_list *list) {
    if (!list) {
        return;
    }

    pthread_mutex_destroy(&list->mutex);

    racs_mt_node *curr = atomic_load_explicit(&list->head, memory_order_relaxed);
    while (curr != NULL) {
        racs_mt_node *next = atomic_load_explicit(&curr->next, memory_order_relaxed);
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
        racs_mt_node *curr_head = atomic_load_explicit(&list->head, memory_order_acquire);

        if (!curr_head) {
            racs_mt_node *new_node = racs_mt_node_create(cfg->memtable.entries);
            if (!new_node) {
                return -1;
            }

            if (racs_mt_list_push_head(list, new_node) != 0) {
                racs_mt_node_destroy(new_node);
                racs_mt_list_flush_tail(list);

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
                racs_mt_list_flush_tail(list);

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

    if (atomic_load_explicit(&mt->is_immutable, memory_order_relaxed)) {
        return 1;
    }

    racs_uint16 slot = atomic_fetch_add_explicit(&mt->num_entries, 1, memory_order_relaxed);

    if (slot >= mt->capacity) {
        atomic_store_explicit(&mt->is_immutable, true, memory_order_relaxed);
        return 1;
    }

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

    atomic_store_explicit(&entry->ready, true, memory_order_release);

    if (slot + 1 == mt->capacity) {
        atomic_store_explicit(&mt->is_immutable, true, memory_order_relaxed);
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

void racs_mt_list_flush_tail(racs_mt_list *list) {
    racs_mt_node *tail = racs_mt_list_pop_tail(list);
    if (tail) {
        racs_enqueue(queue_, tail);
    }
}

int racs_mt_list_push_head(racs_mt_list *list, racs_mt_node *node) {
    if (!list || !node) {
        return -1;
    }

    racs_uint16 current_size = atomic_load_explicit(&list->size, memory_order_relaxed);
    if (current_size >= list->capacity) {
        return -1;
    }

    racs_mt_node *old_head;
    do {
        old_head = atomic_load_explicit(&list->head, memory_order_relaxed);
        atomic_store_explicit(&node->next, old_head, memory_order_relaxed);

    } while (!atomic_compare_exchange_weak_explicit(
                &list->head,
                &old_head,
                node,
                memory_order_release,
                memory_order_relaxed));

    atomic_fetch_add_explicit(&list->size, 1, memory_order_relaxed);
    return 0;
}


racs_mt_node *racs_mt_list_pop_tail(racs_mt_list *list) {
    if (!list) {
        return NULL;
    }

    pthread_mutex_lock(&list->mutex);

    racs_mt_node *head = atomic_load_explicit(&list->head, memory_order_acquire);
    if (!head) {
        pthread_mutex_unlock(&list->mutex);
        return NULL;
    }

    racs_mt_node *next_head = atomic_load_explicit(&head->next, memory_order_acquire);

    if (!next_head) {
        atomic_store_explicit(&list->head, NULL, memory_order_release);
        atomic_fetch_sub_explicit(&list->size, 1, memory_order_relaxed);

        pthread_mutex_unlock(&list->mutex);
        return head;
    }

    racs_mt_node *prev = head;
    racs_mt_node *tail = next_head;

    while (1) {
        racs_mt_node *next_tail = atomic_load_explicit(&tail->next, memory_order_acquire);
        if (!next_tail) {
            break;
        }
        prev = tail;
        tail = next_tail;
    }

    rcu_assign_pointer(prev->next, NULL);
    atomic_fetch_sub_explicit(&list->size, 1, memory_order_relaxed);

    pthread_mutex_unlock(&list->mutex);
    return tail;
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
    atomic_store_explicit(&node->next, NULL, memory_order_relaxed);

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
    atomic_store_explicit(&mt->num_entries, 0, memory_order_relaxed);
    atomic_store_explicit(&mt->is_immutable, false, memory_order_relaxed);

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
        racs_uint16 count = atomic_load_explicit(&mt->num_entries, memory_order_relaxed);

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

    racs_uint16 count = atomic_load_explicit(&mt->num_entries, memory_order_acquire);
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

        while (!atomic_load_explicit(&entry->ready, memory_order_acquire)) {
            #if defined(__x86_64__) || defined(_M_X64)
                __builtin_ia32_pause();
            #elif defined(__aarch64__)
                __asm__ volatile("yield" ::: "memory");
            #endif
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

void racs_mt_node_queue_cb(void *data) {
    (void) data;
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
