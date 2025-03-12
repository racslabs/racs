
#ifndef AUXTS_MERGE_NEON_H
#define AUXTS_MERGE_NEON_H

#include "simd.h"

#ifdef __ARM_NEON__

AUXTS_SIMD float32x4_t neon_merge(float32x4_t _a, float32x4_t _b, float m) {
    const float32x4_t _m = vdupq_n_f32(m);
    return vmulq_f32(vfmaq_f32(_b, _m, _a), vrecpeq_f32(_b));
}
#endif

#endif //AUXTS_MERGE_NEON_H
