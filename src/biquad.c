#include "biquad.h"

racs_int16 *racs_biquad_s16(racs_int16 *in,
                            const char *type,
                            float fc,
                            float fs,
                            float p0,
                            racs_uint16 channels,
                            size_t size) {
    float *f_in = racs_s16_f32(in, size * channels);
    float *f_out = NULL;
    float *coeff = NULL;

    size_t coeff_size;
    if (strcmp(type, "ap") == 0 || strcmp(type, "allpass") == 0)
        coeff = racs_allpass_coeff(fc, p0, fs, &coeff_size);
    if (strcmp(type, "lp") == 0 || strcmp(type, "lowpass") == 0)
        coeff = racs_lowpass_coeff(fc, p0, fs, &coeff_size);
    if (strcmp(type, "hp") == 0 || strcmp(type, "highpass") == 0)
        coeff = racs_highpass_coeff(fc, p0, fs, &coeff_size);

    if (!coeff) {
        racs_free(f_in);
        racs_free(coeff);
        return NULL;
    }

    if (channels == 1)
        f_out = racs_biquad(f_in, coeff, size, coeff_size);
    if (channels == 2)
        f_out = racs_biquad_interleaved(f_in, coeff, size, coeff_size);

    if (!f_out) {
        racs_free(f_in);
        racs_free(coeff);
        return NULL;
    }

    racs_int16 *out = racs_f32_s16(f_out, size * channels);
    racs_free(f_in);
    racs_free(f_out);
    racs_free(coeff);

    return out;
}