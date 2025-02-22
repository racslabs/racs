
#ifndef AUXTS_SIMD_NEON_H
#define AUXTS_SIMD_NEON_H

#include "simd.h"

#ifdef __ARM_NEON__

void auxts_simd_swap24(const int32_t* in, int32_t* out, size_t n);
void auxts_simd_swap32(const int32_t* in, int32_t* out, size_t n);

#endif

#endif //AUXTS_SIMD_NEON_H
