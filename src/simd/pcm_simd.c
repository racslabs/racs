#include "pcm_simd.h"

void auxts_simd_s32_s24(const auxts_int32* q0, auxts_int24* q1, size_t n) {
    AUXTS_UNROLL(8)
    for (int i = 0; i < n; ++i) {
        q1[i]._[0] = (auxts_int8)(q0[i] & 0xff);
        q1[i]._[1] = (auxts_int8)((q0[i] >> 8) & 0xff);
        q1[i]._[2] = (auxts_int8)((q0[i] >> 16) & 0xff);
    }
}

void auxts_simd_s24_s32(const auxts_int24* q0, auxts_int32* q1, size_t n) {
    AUXTS_UNROLL(8)
    for (int i = 0; i < n; ++i) {
        q1[i] = (q0[i]._[0]) | (q0[i]._[1] << 8) | (q0[i]._[2] << 16);
        if (q1[i] & 0x800000) q1[i] |= ~0xffffff;
    }
}
