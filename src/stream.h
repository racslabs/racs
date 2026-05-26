
#ifndef RACS_STREAM_H
#define RACS_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif


#include "mt.h"
#include "mmh3.h"
#include "info.h"
#include "crc32c.h"
#include "racs_zstd.h"


typedef enum {
    RACS_STREAM_OK,
    RACS_STREAM_NOT_FOUND,
    RACS_STREAM_CONFLICT,
    RACS_STREAM_DECODE_ERR,
    RACS_STREAM_ALLOC_ERR
} racs_stream_result;

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


int racs_streams_put(racs_streams *streams,
                     const char *name,
                     size_t size
                     const char *mime_type,
                     const racs_uint8 *src,
                     racs_uint32 src_size);

racs_stream *racs_stream_create(const char *path);

void racs_stream_destroy(racs_stream *stream);

racs_uint8 *racs_stream_decode(const char *mime_type,
                               const racs_uint8 *src,
                               racs_uint32 src_size,
                               racs_uint32 *decoded_size);

racs_uint8 *racs_stream_decode_pcm(const racs_uint8 *src,
                                   racs_uint32 src_size,
                                   racs_uint32 *decoded_size);


#ifdef __cplusplus
}
#endif

#endif //RACS_STREAM_H
