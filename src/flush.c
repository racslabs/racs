
#include "flush.h"


typedef struct {
    char        path[PATH_MAX];
    size_t      size;
    racs_uint8 *data;
} racs_flush_entry;


static racs_queue *flush_q_ = NULL;


racs_flush_entry *racs_flush_entry_create(const char *path, racs_uint8 *data, size_t size);

void racs_flush_destroy_cb(void *data);

void *racs_flush_worker(void* arg);


void racs_flush_queue_init(void) {
    if (!flush_q_) {
        flush_q_ = racs_queue_create(racs_flush_destroy_cb);
        if (!flush_q_) {
            exit(-1);
        }
    }
}

racs_queue *racs_flush_queue_get(void) {
    if (!flush_q_) {
        return NULL;
    }

    return flush_q_;
}

void racs_flush_thread_start(racs_queue *flush_q) {
    pthread_t thread;
    pthread_create(&thread, NULL, racs_flush_worker, flush_q);
    pthread_detach(thread);
}

void *racs_flush_worker(void* arg) {
    racs_queue *flush_q = (racs_queue *) arg;

    for ( ; ; ) {
        racs_queue_entry *q_entry = racs_dequeue(flush_q);
        if (!q_entry) {
            continue;
        }
        
        racs_flush_entry *f_entry = (racs_flush_entry *)q_entry->data;

        racs_fs_mkdir(f_entry->path);
        racs_fs_write(f_entry->path, f_entry->data, f_entry->size);

        racs_queue_entry_destroy(q_entry);
    }

    return NULL;
}   

void racs_flush_enqueue(racs_queue *flush_q, 
                        const char *path, 
                        racs_uint8 *data,
                        size_t size) {
    racs_flush_entry *entry = racs_flush_entry_create(path, data, size);
    if (!entry) {
        return;
    }
    
    racs_enqueue(flush_q, entry);
}

racs_flush_entry *racs_flush_entry_create(const char *path, racs_uint8 *data, size_t size) {
    if (!path || !data) {
        return NULL;
    }

    if ((strlen(path) + 4) > PATH_MAX) {
        return NULL;
    }

    racs_flush_entry *entry = malloc(sizeof(racs_flush_entry));
    if (!entry) {
        return NULL;
    }

    strcpy(entry->path, path);
    entry->data = data;

    return entry;
}

void racs_flush_destroy_cb(void *data) {
    racs_flush_entry *entry = (racs_flush_entry *) data;
    free(entry->data);
    free(entry);
}
