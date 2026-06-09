
#ifndef RACS_FLUSH_H
#define RACS_FLUSH_H

#ifdef __cplusplus
extern "C" {
#endif


#include "fs.h"
#include "queue.h"


void racs_flush_queue_init(void);

racs_queue *racs_flush_queue_get(void);

void racs_flush_enqueue(racs_queue *flush_q, 
                        const char *path, 
                        racs_uint8 *data,
                        size_t size);

void racs_flush_thread_start(racs_queue *flush_q);



#ifdef __cplusplus
}
#endif

#endif //RACS_FLUSH_H