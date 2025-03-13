
#ifndef AUXTS_SCALE_NEON_H
#define AUXTS_SCALE_NEON_H

#include "scale.h"

AUXTS_SIMD float32x4_t neon_scale(int32x4_t _a) {
    const float32x4_t _m = vdupq_n_f32(2147483647.f);
    float32x4_t _b = vcvtq_s32_f32(_a);
    return vmulq_f32(_b, vrecpeq_f32(_m));
}

AUXTS_SIMD int32x4_t neon_unscale(float32x4_t _a) {
    const float32x4_t _m = vdupq_n_f32(2147483647.f);
    return vcvtq_f32_s32(vmulq_f32(_a, _m));
}

#endif //AUXTS_SCALE_NEON_H
