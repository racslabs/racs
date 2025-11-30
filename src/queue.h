
#ifndef RACS_QUEUE_H
#define RACS_QUEUE_H

#include <pthread.h>
#include "types.h"

typedef struct racs_queue_entry {
    size_t size;
    racs_uint8 *data;
    struct racs_queue_entry *next;
} racs_queue_entry;

typedef struct {
    int size;
    racs_queue_entry *head;
    racs_queue_entry *tail;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} racs_queue;

void racs_queue_init(racs_queue *q);

void racs_enqueue(racs_queue *q, size_t size, racs_uint8 *data);

racs_queue_entry *racs_dequeue(racs_queue *q);

racs_queue_entry *racs_queue_entry_create(size_t size, racs_uint8 *data);

void racs_queue_entry_destroy(racs_queue_entry *entry);

#endif //RACS_QUEUE_H