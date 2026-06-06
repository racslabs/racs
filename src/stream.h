
#ifndef RACS_STREAM_H
#define RACS_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif


#include "mt.h"
#include "mmh3.h"
#include "info.h"
#include "offsets.h"
#include "crc32c.h"
#include "racs_zstd.h"


typedef enum {
    RACS_STREAM_OK,
    RACS_STREAM_NOT_FOUND,
    RACS_STREAM_CONFLICT,
    RACS_STREAM_DECODE_ERROR,
    RACS_STREAM_ALLOC_ERROR,
    RACS_STREAM_BUFFER_OVERFLOW
} racs_stream_result;

typedef struct {
    racs_dict       *dict;
    pthread_mutex_t  mutex;
} racs_streams;


extern const char *const racs_stream_result_string[];


void racs_streams_init(void);

racs_streams *racs_streams_get(void);

int racs_streams_create(const char *name,
                        racs_uint32 sample_rate,
                        racs_uint8 channels,
                        racs_uint8 bit_depth);

int racs_streams_open(racs_streams *streams, const char *name);

int racs_streams_append(racs_streams *streams,
                        const char *name,
                        const char *mime_type,
                        const racs_uint8 *src,
                        racs_uint32 src_size);

int racs_streams_close(racs_streams *streams, const char *name);

void racs_streams_destroy(void);

#ifdef __cplusplus
}
#endif

#endif //RACS_STREAM_H
