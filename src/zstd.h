
#ifndef RACS_ZSTD_H
#define RACS_ZSTD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "log.h"
#include <zstd.h>

racs_uint8* racs_zstd_compress(const void* src, size_t src_size, size_t* compressed_size, int compression_level);

racs_uint8* racs_zstd_decompress(const void* src, size_t src_size, size_t* decompressed_size);

#ifdef __cplusplus
}
#endif

#endif //RACS_ZSTD_H