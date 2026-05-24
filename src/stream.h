
#ifndef RACS_STREAM_H
#define RACS_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif


#include "mt.h"
#include "mmh3.h"
#include "info.h"


typedef struct {
    racs_uint8  id[16];
    racs_uint32 size;
    racs_uint32 capacity;
    racs_uint8 *buf;
    racs_info  *info;
} racs_stream;

typedef struct {
    racs_dict       *dict;
    pthread_mutex_t  mutex;
} racs_streams;


racs_stream *racs_stream_create(const char *path);

void racs_stream_destroy(racs_stream *stream);


#ifdef __cplusplus
}
#endif

#endif //RACS_STREAM_H
