#ifndef RACS_QUEUE_H
#define RACS_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif


#include "types.h"
#include <pthread.h>
#include <stdio.h>


typedef void (*racs_queue_destroy_cb) (void *data);

typedef struct racs_queue_entry {
    size_t                   size;
    void                    *data;
    racs_queue_destroy_cb    cb;
    struct racs_queue_entry *next;
} racs_queue_entry;

typedef struct {
    int                   size;
    racs_queue_entry     *head;
    racs_queue_entry     *tail;
    racs_queue_destroy_cb cb;
    pthread_mutex_t       mutex;
    pthread_cond_t        cond;
} racs_queue;


racs_queue *racs_queue_create(racs_queue_destroy_cb cb);

void racs_enqueue(racs_queue *q, void *data);

racs_queue_entry *racs_dequeue(racs_queue *q);

void racs_queue_entry_destroy(racs_queue_entry *entry);

void racs_queue_destroy(racs_queue *q);

int racs_queue_size(racs_queue *q);

#ifdef __cplusplus
}
#endif

#endif //RACS_QUEUE_H
