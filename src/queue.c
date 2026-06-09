
#include "queue.h"


racs_queue_entry *racs_queue_entry_create(racs_queue_destroy_cb cb, void *data);


racs_queue *racs_queue_create(racs_queue_destroy_cb cb) {
    racs_queue *q = malloc(sizeof(racs_queue));
    if (!q) {
        return NULL;
    }

    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);

    q->cb = cb;
    q->head = q->tail = NULL;
    q->size = 0;

    return q;
}

racs_queue_entry *racs_queue_entry_create(racs_queue_destroy_cb cb, void *data) {
    racs_queue_entry *entry = malloc(sizeof(racs_queue_entry));
    if (!entry) {
        return NULL;
    }

    entry->cb = cb;
    entry->data = data;
}

void racs_enqueue(racs_queue *q, void *data) {
    racs_queue_entry *entry = racs_queue_entry_create(q->cb, data);
    if (!entry) {
        return;
    }

    pthread_mutex_lock(&q->mutex);

    if (!q->tail) {
        q->head = q->tail = entry;
    } else {
        q->tail->next = entry;
        q->tail = entry;
    }

    ++q->size;
    printf("e %zu\n", q->size);

    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

racs_queue_entry *racs_dequeue(racs_queue *q) {
    pthread_mutex_lock(&q->mutex);
    printf("e %zu\n", q->size);
    while (!q->head) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }

    racs_queue_entry *entry = q->head;
    q->head = q->head->next;

    if (!q->head) {
        q->tail = NULL;
    }

    --q->size;

    pthread_mutex_unlock(&q->mutex);
    return entry;
}

void racs_queue_entry_destroy(racs_queue_entry *entry) {
    racs_queue_destroy_cb cb = entry->cb;
    if (cb) {
        cb(entry->data);
    }

    free(entry);
}

int racs_queue_size(racs_queue *q) {
    return q->size;
}

void racs_queue_destroy(racs_queue *q) {
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
    
    free(q);
}
