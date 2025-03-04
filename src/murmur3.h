
#ifndef AUXTS_MURMUR3_H
#define AUXTS_MURMUR3_H

#include <stdint.h>
#include "endian.h"
#include "export.h"

#ifdef __cplusplus
extern "C" {
#endif

AUXTS_FORCE_INLINE uint64_t rotl64(uint64_t x, int8_t r) {
    return (x << r) | (x >> (64 - r));
}

AUXTS_FORCE_INLINE uint64_t fmix64(uint64_t k) {
    k ^= k >> 33;
    k *= 0xff51afd7ed558ccdull;
    k ^= k >> 33;
    k *= 0xc4ceb9fe1a85ec53ull;
    k ^= k >> 33;

    return k;
}

void murmur3_x64_128(const uint8_t* key, size_t len, uint32_t seed, uint64_t* out);

#ifdef __cplusplus
}
#endif

#endif //AUXTS_MURMUR3_H
