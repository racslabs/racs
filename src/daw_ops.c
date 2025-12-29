#include "daw_ops.h"

racs_int32 *racs_daw_ops_mix(
    const racs_int32 *in_a,
    size_t in_a_len,
    const racs_int32 *in_b,
    size_t in_b_len,
    size_t *out_len
) {
    if (!in_a || !in_b || !out_len) return NULL;

    *out_len = (in_a_len > in_b_len) ? in_a_len : in_b_len;
    racs_int32 *out = (racs_int32 *)calloc(*out_len, sizeof(racs_int32));
    if (!out) return NULL;

    for (size_t i = 0; i < *out_len; i++) {
        float sample_a = (i < in_a_len) ? (float)in_a[i] : 0.0f;
        float sample_b = (i < in_b_len) ? (float)in_b[i] : 0.0f;

        float mixed = sample_a + sample_b;
        out[i] = (racs_int32)mixed;
    }

    return out;
}

racs_int32 *racs_daw_ops_gain(
    const racs_int32 *in,
    size_t in_len,
    double gain
) {
    if (!in) return NULL;

    racs_int32 *out = calloc(in_len, sizeof(racs_int32));
    if (!out) return NULL;

    for (size_t i = 0; i < in_len; i++)
        out[i] = (racs_int32)(gain *in[i]);

    return out;
}
