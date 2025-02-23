#include "simd_neon.h"

#ifdef __ARM_NEON__

static uint16x8_t simd_neon_swap16(uint16x8_t in);

uint16x8_t simd_neon_swap16(uint16x8_t in) {
    const uint16x8_t mask1 = vdupq_n_u16(0xff00);
    const uint16x8_t mask2 = vdupq_n_u16(0x00ff);

    uint16x8_t b1 = vandq_u16(vshlq_n_u16(in, 8), mask1);
    uint16x8_t b2 = vandq_u16(vshrq_n_u16(in, 8), mask2);

    return vornq_u16(b1, b2);
}

void auxts_simd_swap16(const int16_t* in, int16_t* out, size_t n) {
    size_t i = 0;
    size_t m = n & ~7;
//
//    for ( ; i < m; i += 8) {
//        int16x8_t _in = vld1q_s16(in + i);
//        uint16x8_t _out = simd_neon_swap16(_in);
//        vst1q_s16(out + i, _out);
//    }

    for ( ; i < n; ++i) {
        out[i] = (int16_t)__builtin_bswap16(in[i]);
    }
}

#endif