#include "pcm_simd.h"

#ifdef __ARM_NEON__

void auxts_simd_interleave_s16(const auxts_int16* q0, const auxts_int16* q1, auxts_int16* q2, size_t n) {
    size_t i = 0;
    size_t m = n & ~3;

    for ( ; i < m; i += 4) {
        int16x8_t _q0 = vld1q_s16(q0 + i);
        int16x8_t _q1 = vld1q_s16(q1 + i);
        int16x8_t _q2 = vzip1q_s16(_q0, _q1);
        vst1q_s16(q2 + i * 2, _q2);
    }

    for ( ; i < n; ++i) {
        q2[i * 2    ] = q0[i];
        q2[i * 2 + 1] = q1[i];
    }
}

void auxts_simd_planar_s16(const auxts_int16* q0, auxts_int16* q1, size_t n) {
    AUXTS_UNROLL(8)
    for (int i = 0; i < n / 2; ++i) {
        q1[i] = q0[i * 2];
        q1[i + n / 2] = q0[i * 2 + 1];
    }
}

#endif
