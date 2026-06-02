
#ifndef RACS_OFFSETS_H
#define RACS_OFFSETS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mmh3.h"
#include "dict.h"
#include <pthread.h>


#define RACS_OFFSETS_PUT(hash, offset) \
do { \
    if (racs_offsets_get()) { \
        racs_offsets_put(racs_offsets_get_(), hash, offset); \
    } \
} while(0)

#define RACS_OFFSETS_GET(hash) \
    (racs_offsets_get() != NULL) ? racs_offsets_get(racs_offsets_get_(), hash) : 0;


typedef struct {
    racs_dict       *dict;
    pthread_mutex_t  mutex;
} racs_offsets;


void racs_offsets_init(void);

void racs_offsets_put_offset(racs_offsets *offsets, racs_uint64 hash, racs_uint64 offset);

racs_uint64 racs_offsets_get_offset(racs_offsets *offsets, racs_uint64 hash);

racs_offsets *racs_offsets_get(void);

#ifdef __cplusplus
}
#endif

#endif //RACS_OFFSETS_H