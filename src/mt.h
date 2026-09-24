// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//

#ifndef RACS_MT_H
#define RACS_MT_H

#ifdef __cplusplus
extern "C" {
#endif


#include "config.h"
#include "dict.h"
#include "types.h"
#include "path.h"
#include "fs.h"
#include "queue.h"
#include "sst.h"
#include <stdio.h>
#include <pthread.h>
#include <urcu-qsbr.h>
#include <stdatomic.h>
#include <stdbool.h>


typedef struct {
    racs_uint64 key[3];
    racs_uint64 lsn;
    racs_uint32 block_size;
    racs_uint32 checksum;
    racs_uint8 *block;

    _Atomic bool ready; 
} racs_mt_entry;

typedef struct {
    _Atomic racs_uint16 num_entries;
    _Atomic bool is_immutable;
    racs_uint16 capacity;
    racs_mt_entry *entries;
} racs_mt;

typedef struct racs_mt_node {
    racs_mt *mt;
    _Atomic (struct racs_mt_node *) next; 
    
    struct rcu_head rcu;
} racs_mt_node;

typedef struct {
    racs_uint16 capacity;   
    pthread_mutex_t mutex;          
    _Atomic racs_uint16 size;         
    _Atomic (racs_mt_node *) head;   
} racs_mt_list;

typedef struct {
    int capacity;
    racs_dict *dict;
} racs_mt_parts;


typedef int (*racs_mt_list_iter_cb)(const racs_mt_entry *entry, void *data);


void racs_mt_list_init(void);

racs_mt_list *racs_mt_list_get(void);

void racs_mt_list_iter(racs_mt_list *list, racs_mt_list_iter_cb cb, void *data);

void racs_mt_flush_thread_start(void);

void racs_mt_list_flush_tail(racs_mt_list *list);

int racs_mt_list_put(racs_mt_list *list,
                     const racs_uint64 *key,
                     const racs_uint8 *block,
                     racs_uint16 block_size,
                     racs_uint32 checksum,
                     racs_uint64 lsn);


#ifdef __cplusplus
}
#endif

#endif //RACS_MT_H
