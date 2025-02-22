#include "simd_neon.h"

#ifdef __ARM_NEON__

static uint32x4_t simd_neon_swap24(uint32x4_t in);

uint32x4_t simd_neon_swap24(uint32x4_t in) {
    const uint32x4_t low  = vdupq_n_u32(0x0000ff);
    const uint32x4_t mid  = vdupq_n_u32(0x00ff00);
    const uint32x4_t high = vdupq_n_u32(0xff0000);

    uint32x4_t b1 = vandq_u32(vshrq_n_u32(in, 16), low);
    uint32x4_t b2 = vandq_u32(in, mid);
    uint32x4_t b3 = vandq_u32(vshlq_n_u32(in, 16), high);

    return vorrq_u32(vorrq_u32(b1, b2), b3);
}

void auxts_simd_swap24(const int32_t* in, int32_t* out, size_t n) {
    size_t i = 0;
    size_t m = n & ~3;

    for ( ; i < m; i += 4) {
        int32x4_t _in = vld1q_s32(in + i);
        uint32x4_t _out = simd_neon_swap24(_in);
        vst1q_s32(out + i, vreinterpretq_u32_s32(_out));
    }

    for ( ; i < n; ++i) {
        uint32_t _in = (uint32_t) in[i];
        out[i] = (int32_t)((((_in) >> 16) & 0xff) | (((_in)) & 0xff00) | (((_in) & 0xff) << 16));
    }
}
#endif
