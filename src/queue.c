#include "queue.h"

#include "log.h"

void racs_queue_init(racs_queue *q) {
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
    q->head = q->tail = NULL;
    q->size = 0;
}

racs_queue_entry *racs_queue_entry_create(size_t size, racs_uint8 *data) {
    racs_queue_entry *entry = malloc(sizeof(racs_queue_entry));
    if (entry == NULL) {
        racs_log_error("racs_queue_entry_create failed");
        return NULL;
    }

    entry->size = size;
    entry->data = malloc(size);

    if (entry->data == NULL) {
        racs_log_error("failed to allocate data to racs_queue_entry");
        free(entry);
        return NULL;
    }

    memcpy(entry->data, data, size);
    entry->next = NULL;
    return entry;
}

void racs_enqueue(racs_queue *q, size_t size, racs_uint8 *data) {
    racs_queue_entry *entry = racs_queue_entry_create(size, data);
    if (!entry) return;

    pthread_mutex_lock(&q->mutex);

    if (q->tail == NULL) {
        q->head = q->tail = entry;
    } else {
        q->tail->next = entry;
        q->tail = entry;
    }

    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->mutex);
}

racs_queue_entry *racs_dequeue(racs_queue *q) {
    pthread_mutex_lock(&q->mutex);

    while (q->head == NULL)
        pthread_cond_wait(&q->cond, &q->mutex);

    racs_queue_entry *temp = q->head;
    q->head = q->head->next;
    if (q->head == NULL) q->tail = NULL;

    pthread_mutex_unlock(&q->mutex);
    return temp;
}

void racs_queue_entry_destroy(racs_queue_entry *entry) {
    free(entry->data);
    free(entry);
}

void racs_queue_destroy(racs_queue *q) {
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond);
}