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