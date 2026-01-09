#include "zstd.h"

racs_uint8* racs_zstd_compress(const void* src, size_t src_size, size_t* compressed_size, int compression_level) {
    size_t bound = ZSTD_compressBound(src_size);
    racs_uint8* out = malloc(bound);
    if (!out) return NULL;

    size_t rc = ZSTD_compress(out, bound, src, src_size, compression_level);
    if (ZSTD_isError(rc)) {
        racs_log_error("Compression error: %s", ZSTD_getErrorName(rc));
        free(out);
        return NULL;
    }

    *compressed_size = rc;
    return out;
}

racs_uint8* racs_zstd_decompress(const void* src, size_t src_size, size_t* decompressed_size) {
    racs_uint64 original_size = ZSTD_getFrameContentSize(src, src_size);
    if (original_size == ZSTD_CONTENTSIZE_ERROR || original_size == ZSTD_CONTENTSIZE_UNKNOWN) {
        racs_log_error("Invalid or unknown decompressed size");
        return NULL;
    }

    racs_uint8* out = malloc(original_size);
    if (!out) return NULL;

    size_t rc = ZSTD_decompress(out, original_size, src, src_size);
    if (ZSTD_isError(rc)) {
        racs_log_error("Decompression error: %s", ZSTD_getErrorName(rc));
        free(out);
        return NULL;
    }

    *decompressed_size = rc;
    return out;
}
