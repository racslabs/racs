#include "ops.h"

racs_int32 *racs_ops_mix(
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

racs_int32 *racs_ops_gain(
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

racs_int32 *racs_ops_trim(
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

racs_int32 *racs_ops_fade(
    const racs_int32 *in,
    size_t in_len,
    double fade_in_seconds,
    double fade_out_seconds,
    size_t *out_len
) {
    if (!in || in_len < 2) return NULL;

    racs_uint32 sample_rate = in[0];
    racs_uint16 channels    = (racs_uint16)(in[1] >> 16);

    *out_len = in_len;

    racs_int32 *out = calloc(*out_len, sizeof(racs_int32));
    if (!out) return NULL;

    // pre-pend sample-rate, channels and bit-depth
    out[0] = in[0];
    out[1] = in[1];

    size_t start = 2;
    size_t len   = in_len - start;

    size_t fade_in_samples  = (size_t)(fade_in_seconds  * sample_rate * channels);
    size_t fade_out_samples = (size_t)(fade_out_seconds * sample_rate * channels);

    if (fade_in_samples > len) fade_in_samples = len;
    if (fade_out_samples > len) fade_out_samples = len;

    for (size_t i = 0; i < len; i++) {
        double gain = 1.0;

        // Fade in
        if (i < fade_in_samples) {
            gain = (double)i / fade_in_samples;
        }

        // Fade out
        if (i >= len - fade_out_samples) {
            size_t fade_out_pos = i - (len - fade_out_samples);
            double fade_gain = 1.0 - ((double)fade_out_pos / fade_out_samples);
            if (fade_gain < gain) gain = fade_gain;
        }

        out[start + i] = (racs_int32)(in[start + i] * gain);
    }

    return out;
}

racs_int32 *racs_ops_pan(
    const racs_int32 *in,
    size_t in_len,
    double pan,
    size_t *out_len
) {
    if (!in || in_len < 2) return NULL;

    racs_uint16 channels    = (racs_uint16)(in[1] >> 16);
    if (channels < 2) return NULL;

    *out_len = in_len;
    racs_int32 *out = calloc(*out_len, sizeof(racs_int32));
    if (!out) return NULL;

    // pre-pend sample-rate, channels and bit-depth
    out[0] = in[0];
    out[1] = in[1];

    size_t start = 2;
    size_t len   = in_len - start;

    for (size_t i = 0; i < len; i += channels) {
        racs_int32 left  = in[start + i + 0];
        racs_int32 right = in[start + i + 1];

        double left_gain  = pan <= 0 ? 1.0 : 1.0 - pan;
        double right_gain = pan >= 0 ? 1.0 : 1.0 + pan;

        out[start + i + 0] = (racs_int32)(left  * left_gain);
        out[start + i + 1] = (racs_int32)(right * right_gain);
    }

    return out;
}

racs_int32 *racs_ops_pad(
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

    size_t left_pad  = (size_t)(left_seconds  * sample_rate * channels);
    size_t right_pad = (size_t)(right_seconds * sample_rate * channels);

    *out_len = start + left_pad + len + right_pad;

    racs_int32 *out = calloc(*out_len, sizeof(racs_int32));
    if (!out) return NULL;

    // pre-pend sample-rate, channels and bit-depth
    out[0] = in[0];
    out[1] = in[1];

    memcpy(out + start + left_pad, in  + start, len * sizeof(racs_int32));

    return out;
}

racs_int32 *racs_ops_clip(
    const racs_int32 *in,
    size_t in_len,
    racs_int32 min_val,
    racs_int32 max_val,
    size_t *out_len
) {
    if (!in || in_len < 2) return NULL;
    if (min_val > max_val) return NULL;

    *out_len = in_len;

    racs_int32 *out = malloc(in_len * sizeof(racs_int32));
    if (!out) return NULL;

    // pre-pend sample-rate, channels and bit-depth
    out[0] = in[0];
    out[1] = in[1];

    size_t start = 2;

    for (size_t i = start; i < in_len; i++) {
        racs_int32 x = in[i];
        if (x < min_val) x = min_val;
        else if (x > max_val) x = max_val;
        out[i] = x;
    }

    return out;
}

racs_int32 *racs_ops_split(
    const racs_int32 *in,
    size_t in_len,
    racs_uint16 channel,
    size_t *out_len
) {
    if (!in || in_len < 2 || !out_len)
        return NULL;

    racs_uint16 channels = (racs_uint16)(in[1] >> 16);
    if (channels != 2) return NULL;

    size_t start = 2;
    size_t len   = in_len - start;

    size_t frames = len / channels;
    *out_len = start + frames;

    racs_int32 *out = malloc(*out_len * sizeof(racs_int32));
    if (!out) return NULL;

    out[0] = in[0];
    out[1] = (racs_int32)(in[1] & 0x0000ffff | 1u << 16);

    for (size_t f = 0; f < frames; f++)
        out[start + f] = in[start + f * channels + channel];

    return out;
}

racs_int32 *racs_ops_merge(
    const racs_int32 *in_a,
    size_t in_len_a,
    const racs_int32 *in_b,
    size_t in_len_b,
    size_t *out_len
) {
    if (!in_a || !in_b || !out_len) return NULL;
    if (in_len_a < 2 || in_len_b < 2) return NULL;

    // both must be mono
    racs_uint16 channel_a = (racs_uint16)(in_a[1] >> 16);
    racs_uint16 channel_b = (racs_uint16)(in_b[1] >> 16);
    if (channel_a != 1 || channel_b != 1) return NULL;

    if (in_a[0] != in_b[0] || (in_a[1] & 0x0000ffff) != (in_b[1] & 0x0000ffff))
        return NULL;

    size_t start = 2;
    size_t frames_a = in_len_a - start;
    size_t frames_b = in_len_b - start;

    size_t frames = frames_a > frames_b ? frames_a : frames_b;
    *out_len = start + frames * 2;

    racs_int32 *out = calloc(*out_len, sizeof(racs_int32));
    if (!out) return NULL;

    out[0] = in_a[0];
    out[1] = (racs_int32)(in_a[1] & 0x0000ffff | 2u << 16);

    for (size_t f = 0; f < frames; f++) {
        if (f < frames_a)
            out[start + 2*f + 0] = in_a[start + f];
        // else zero (calloc)

        if (f < frames_b)
            out[start + 2*f + 1] = in_b[start + f];
    }

    return out;
}
