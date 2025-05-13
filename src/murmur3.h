
#ifndef RACS_MURMUR3_H
#define RACS_MURMUR3_H

#include <stdint.h>
#include <stdlib.h>
#include "export.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

RACS_FORCE_INLINE racs_uint64 rotl64(racs_uint64 x, racs_int8 r) {
    return (x << r) | (x >> (64 - r));
}

RACS_FORCE_INLINE racs_uint64 fmix64(racs_uint64 k) {
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdull;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53ull;
    k ^= k >> 33;

    return k;
}

void murmur3_x64_128(const racs_uint8 *key, size_t len, racs_uint32 seed, racs_uint64 *out);

#ifdef __cplusplus
}
#endif

#endif //RACS_MURMUR3_H
