#ifndef RACS_ZSTD_H
#define RACS_ZSTD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include <zstd.h>


typedef enum {
    RACS_ZSTD_OK,
    RACS_ZSTD_ALLOC_ERROR,
    RACS_ZSTD_COMPRESS_ERROR,
    RACS_ZSTD_DECOMPRESS_ERROR
} racs_zstd_result;

int racs_zstd_compress(const void *src,
                       size_t src_size,
                       void **out,
                       size_t *out_size,
                       int level);

int racs_zstd_decompress(const void *src,
                         size_t src_size,
                         void **out,
                         size_t *out_size);


#ifdef __cplusplus
}
#endif

#endif //RACS_ZSTD_H
