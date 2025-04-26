
#ifndef RATS_MURMUR3_H
#define RATS_MURMUR3_H

#include <stdint.h>
#include <stdlib.h>
#include "export.h"
#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

RATS_FORCE_INLINE rats_uint64 rotl64(rats_uint64 x, rats_int8 r) {
    return (x << r) | (x >> (64 - r));
}

RATS_FORCE_INLINE rats_uint64 fmix64(rats_uint64 k) {
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdull;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53ull;
    k ^= k >> 33;

    return k;
}

void murmur3_x64_128(const rats_uint8* key, size_t len, rats_uint32 seed, rats_uint64* out);

#ifdef __cplusplus
}
#endif

#endif //RATS_MURMUR3_H
