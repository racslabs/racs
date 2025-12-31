#include "daw_ops.h"

#include "log.h"

racs_int32 *racs_daw_ops_mix(
    const racs_int32 *in_a,
    size_t in_a_len,
    const racs_int32 *in_b,
    size_t in_b_len,
    size_t *out_len
) {
    if (!in_a || !in_b || !out_len) return NULL;

    *out_len = in_a_len > in_b_len ? in_a_len : in_b_len;
    racs_int32 *out = calloc(*out_len, sizeof(racs_int32));
    if (!out) return NULL;

    // pre-pend sample-rate, channels and bit-depth
    out[0] = in_a[0];
    out[1] = in_a[1];

    for (size_t i = 2; i < *out_len; i++) {
        float sample_a = i < in_a_len ? (float)in_a[i] : 0.0f;
        float sample_b = i < in_b_len ? (float)in_b[i] : 0.0f;

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

    // pre-pend sample-rate, channels and bit-depth
    out[0] = in[0];
    out[1] = in[1];

    for (size_t i = 2; i < in_len; i++)
        out[i] = (racs_int32)(gain *in[i]);

    return out;
}

racs_int32 *racs_daw_ops_trim(
    const racs_int32 *in,
    size_t in_len,
    double left_seconds,
    double right_seconds,
    size_t *out_len
) {
    if (!in || in_len < 2) return NULL;

    racs_uint32 sample_rate = in[0];
    racs_uint16 channels    = (racs_uint16)(in[1] >> 16);

    size_t start = 2;
    size_t len   = in_len - start;

    size_t left_trim  = (size_t)(left_seconds  * sample_rate * channels);
    size_t right_trim = (size_t)(right_seconds * sample_rate * channels);

    if (left_trim + right_trim >= len)
        return NULL;

    *out_len = start + (len - left_trim - right_trim);

    racs_int32 *out = calloc(*out_len, sizeof(racs_int32));
    if (!out) return NULL;

    // pre-pend sample-rate, channels and bit-depth
    out[0] = in[0];
    out[1] = in[1];

    const size_t copy_len = *out_len - start;
    memcpy(out + start, in  + start + left_trim, copy_len * sizeof(racs_int32));

    return out;
}

