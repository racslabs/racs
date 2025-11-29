#include "queue.h"

#include "log.h"

racs_queue *racs_queue_create() {
    racs_queue *q = malloc(sizeof(racs_queue));
    if (q == NULL) {
        racs_log_error("racs_queue_create failed");
        return NULL;
    }

    q->head = q->tail = NULL;
    q->size = 0;
    return q;
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
    return entry;
}

void enqueue(racs_queue *q, size_t size, racs_uint8 *data) {
    racs_queue_entry *entry = racs_queue_entry_create(size, data);
    if (q->tail == NULL) {
        q->head = q->tail = entry;
        return;
    }

    q->tail->next = (struct racs_queue_entry *)entry;
    q->tail = entry;
}

racs_queue_entry *dequeue(racs_queue *q) {
    if (q->head == NULL) return NULL;

    racs_queue_entry *temp = q->head;

    q->head = (racs_queue_entry *)q->head->next;
    if (q->head == NULL) q->tail = NULL;

    return temp;
}

void racs_queue_entry_destroy(racs_queue_entry *entry) {
    free(entry->data);
    free(entry);
}