#include "convert.h"

#ifdef __ARM_NEON__

void simd_convert_float32_to_int32(float* a, int32_t* b, size_t n) {
    size_t i = 0;
    size_t k = n & ~3;

    for ( ; i < k; i += 4) {
        float32x4_t _a = vld1q_f32(a + i);
        int32x4_t _b = vcvtq_f32_s32(_a);
        vst1q_s32(b + i, _b);
    }

    for ( ; i < n; ++i) {
        b[i] = (int)a[i];
    }
}

void simd_convert_int32_to_float32(int32_t* a, float* b, size_t n) {
    size_t i = 0;
    size_t k = n & ~3;

    for ( ; i < k; i += 4) {
        int32x4_t _a = vld1q_s32(a + i);
        float32x4_t _b = vcvtq_s32_f32(_a);
        vst1q_f32(b + i, _b);
    }

    for ( ; i < n; ++i) {
        b[i] = (float)a[i];
    }
}
#endif
