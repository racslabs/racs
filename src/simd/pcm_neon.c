#include "pcm_simd.h"

#ifdef __ARM_NEON__

void auxts_simd_interleave_s32(const auxts_int32* q0, const auxts_int32* q1, auxts_int32* q2, size_t n) {
    size_t i = 0;
    size_t m = n & ~3;

    for ( ; i < m; i += 2) {
        int32x4_t _q0 = vld1q_s32(q0 + i);
        int32x4_t _q1 = vld1q_s32(q1 + i);
        int32x4_t _q2 = vzip1q_f32(_q0, _q1);
        vst1q_s32(q2 + i * 2, _q2);
    }

    for ( ; i < n; ++i) {
        q2[i * 2    ] = q0[i];
        q2[i * 2 + 1] = q1[i];
    }
}

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

void auxts_simd_s32_s16(const auxts_int32* q0, auxts_int16* q1, size_t n) {
    size_t i = 0;
    size_t m = n & ~3;

    for ( ; i < m; i += 4) {
        int32x4_t _q0 = vld1q_s32(q0 + i);
        int16x4_t _q1  = vmovn_s32(_q0);
        vst1_s16(q1 + i, _q1);
    }

    for ( ; i < n; ++i) {
        q1[i] = (auxts_int16)q0[i];
    }
}

void auxts_simd_s16_s32(const auxts_int16* q0, auxts_int32* q1, size_t n) {
    size_t i = 0;
    size_t m = n & ~3;

    for ( ; i < m; i += 4) {
        int16x4_t _q0 = vld1_s16(q0 + i);
        int32x4_t _q1 = vmovl_s16(_q0);
        vst1q_s32(q1 + i, _q1);
    }

    for ( ; i < n; ++i) {
        q1[i] = (auxts_int32)q0[i];
    }
}
#endif
