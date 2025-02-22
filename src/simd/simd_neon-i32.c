#include "simd_neon.h"

#ifdef __ARM_NEON__

static uint32x4_t simd_neon_swap24(uint32x4_t in);
static uint32x4_t simd_neon_swap32(uint32x4_t in);

uint32x4_t simd_neon_swap24(uint32x4_t in) {
    const uint32x4_t mask1  = vdupq_n_u32(0x0000ff);
    const uint32x4_t mask2  = vdupq_n_u32(0x00ff00);
    const uint32x4_t mask3 = vdupq_n_u32(0xff0000);

    uint32x4_t b1 = vandq_u32(vshrq_n_u32(in, 16), mask1);
    uint32x4_t b2 = vandq_u32(in, mask2);
    uint32x4_t b3 = vandq_u32(vshlq_n_u32(in, 16), mask3);

    return vorrq_u32(vorrq_u32(b1, b2), b3);
}

uint32x4_t simd_neon_swap32(uint32x4_t in) {
    const uint32x4_t mask1 = vdupq_n_u32(0xff000000);
    const uint32x4_t mask2 = vdupq_n_u32(0x00ff0000);
    const uint32x4_t mask3 = vdupq_n_u32(0x0000ff00);
    const uint32x4_t mask4 = vdupq_n_u32(0x000000ff);

    uint32x4_t b1 = vandq_u32(vshlq_n_u32(in, 24), mask1);
    uint32x4_t b2 = vandq_u32(vshlq_n_u32(in, 8), mask2);
    uint32x4_t b3 = vandq_u32(vshrq_n_u32(in, 8), mask3);
    uint32x4_t b4 = vandq_u32(vshrq_n_u32(in, 24), mask4);

    return vorrq_u32(vorrq_u32(vorrq_u32(b1, b2), b3), b4);
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

void auxts_simd_swap32(const int32_t* in, int32_t* out, size_t n) {
    size_t i = 0;
    size_t m = n & ~3;

    for ( ; i < m; i += 4) {
        int32x4_t _in = vld1q_s32(in + i);
        uint32x4_t _out = simd_neon_swap32(_in);
        vst1q_s32(out + i, vreinterpretq_u32_s32(_out));
    }

    for ( ; i < m; ++i) {
        out[i] = (int32_t)__builtin_bswap32(in[i]);
    }
}

#endif
