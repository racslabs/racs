#include "types.h"

float *racs_s16_f32(const racs_int16 *in, size_t n) {
    float *out = racs_malloc(sizeof(float) * n);

    for (int i = 0; i < n; ++i)
        out[i] = (float)in[i] / 32767;

    return out;
}

racs_int16 *racs_f32_s16(const float *in, size_t n) {
    racs_int16 *out = malloc(sizeof(racs_int16) * n);

    for (int i = 0; i < n; ++i)
        out[i] = (racs_int16)(in[i] * 32767);

    return out;
}

racs_int32 *racs_s16_s32(const racs_int16 *in, size_t n) {
    racs_int32 *out = malloc(sizeof(racs_int32) * n);

    for (int i = 0; i < n; ++i)
        out[i] = in[i];

    return out;
}

racs_int32 *racs_s24_s32(const racs_int24 *in, size_t n) {
    racs_int32 *out = malloc(sizeof(racs_int32) * n);

    for (int i = 0; i < n; ++i) {
        int32_t s = (in[i].l | (in[i].m << 8) | (in[i].h << 16));
        if (s & 0x800000) s |= ~0xffffff;
        out[i] = s;
    }

    return out;
}

racs_int24 *racs_s32_s24(const racs_int32 *in, size_t n) {
    racs_int24 *out = malloc(sizeof(racs_int24) * n);

    for (int i = 0; i < n; ++i) {
        racs_int32 s = in[i];

        out[i].l = (racs_int8)(s & 0xff);
        out[i].m = (racs_int8)((s >> 8) & 0xff);
        out[i].h = (racs_int8)((s >> 16) & 0xff);
    }

    return out;
}

racs_int16 *racs_s32_s16(const racs_int32 *in, size_t n) {
    racs_int16 *out = malloc(sizeof(racs_int16) * n);

    for (int i = 0; i < n; ++i)
        out[i] = (racs_int16)in[i];

    return out;
}

racs_int16 *racs_s32_s16_d8(const racs_int32 *in, size_t n) {
    racs_int16 *out = malloc(sizeof(racs_int16) * n);

    for (int i = 0; i < n; ++i)
        out[i] = (racs_int16)(in[i] >> 8);

    return out;
}