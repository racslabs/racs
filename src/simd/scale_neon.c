#include "scale_neon.h"

#ifdef __ARM_NEON__

void simd_scale(int* a, float* b, size_t n) {
    size_t i = 0;
    size_t k = n & ~3;

    for ( ; i < k; i += 4) {
        int32x4_t _a = vld1q_s32(a + i);
        float32x4_t _b = neon_scale(_a);
        vst1q_f32(b + i, _b);
    }

    for ( ; i < n; ++i) {
        b[i] = (float)a[i] / 2147483647.f;
    }
}

void simd_unscale(float* a, int* b, size_t n) {
    size_t i = 0;
    size_t k = n & ~3;

    for ( ; i < k; i += 4) {
        float32x4_t _a = vld1q_f32(a + i);
        int32x4_t _b = neon_unscale(_a);
        vst1q_s32(b + i, _b);
    }

    for ( ; i < n; ++i) {
        b[i] = (int)(a[i] * 2147483647.f);
    }
}

#endif
