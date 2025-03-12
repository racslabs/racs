#include "merge_neon.h"

#ifdef __ARM_NEON__

void simd_merge(float* a, float* b, float* c, size_t n, int m) {
    size_t i = 0;
    size_t k = n & ~3;

    for ( ; i < k; i += 4) {
        float32x4_t _a = vld1q_f32(a + i);
        float32x4_t _b = vld1q_f32(b + i);
        float32x4_t _c = neon_merge(_a, _b, (float)m);
        vst1q_f32(c + i, _c);
    }

    for (; i < n; ++i) {
        c[i] = (b[i] + (float)m * a[i]) / b[i];
    }

}
#endif
