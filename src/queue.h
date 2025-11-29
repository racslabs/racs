
#ifndef RACS_QUEUE_H
#define RACS_QUEUE_H

#include <pthread.h>
#include "types.h"

typedef struct {
    size_t size;
    racs_uint8 *data;
    struct racs_queue_entry *next;
} racs_queue_entry;

typedef struct {
    int size;
    racs_queue_entry *head;
    racs_queue_entry *tail;
} racs_queue;

racs_queue *racs_queue_create();

void enqueue(racs_queue *q, size_t size, racs_uint8 *data);

racs_queue_entry *dequeue(racs_queue *q);

racs_queue_entry *racs_queue_entry_create(size_t size, racs_uint8 *data);

void racs_queue_entry_destroy(racs_queue_entry *entry);

#endif //RACS_QUEUE_H