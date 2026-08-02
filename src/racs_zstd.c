#include "racs_zstd.h"


int racs_zstd_compress(const void *src,
                       size_t src_size,
                       void **out,
                       size_t *out_size,
                       int level) {
    *out = NULL;
    *out_size = 0;

    size_t bound = ZSTD_compressBound(src_size);

    void *buf = malloc(bound);
    if (!buf) {
        return RACS_ZSTD_ALLOC_ERROR;
    }

    size_t rc = ZSTD_compress(buf, bound, src, src_size, level);
    if (ZSTD_isError(rc)) {
        free(buf);
        return RACS_ZSTD_COMPRESS_ERROR;
    }

    *out = buf;
    *out_size = rc;

    return RACS_ZSTD_OK;
}

int racs_zstd_decompress(const void *src,
                         size_t src_size,
                         void **out,
                         size_t *out_size) {
    *out = NULL;
    *out_size = 0;

    racs_uint64 original_size = ZSTD_getFrameContentSize(src, src_size);
    if (original_size == ZSTD_CONTENTSIZE_ERROR || original_size == ZSTD_CONTENTSIZE_UNKNOWN) {
        return RACS_ZSTD_DECOMPRESS_ERROR;
    }

    void *buf = malloc(original_size);
    if (!buf) {
        return RACS_ZSTD_ALLOC_ERROR;
    }

    size_t rc = ZSTD_decompress(out, original_size, src, src_size);
    if (ZSTD_isError(rc)) {
        free(buf);
        return RACS_ZSTD_DECOMPRESS_ERROR;
    }

    *out = buf;
    *out_size = rc;

    return RACS_ZSTD_OK;
}
