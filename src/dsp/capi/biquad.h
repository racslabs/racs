
#ifndef RACS_BIQUAD_H
#define RACS_BIQUAD_H

#include "../signal/biquad.hpp"
#include "../signal/biquad_design.hpp"

#ifdef __cplusplus
extern "C" {
#endif

float *racs_biquad(float *in, float *coeff, size_t in_size, size_t coeff_size);

float *racs_biquad_interleaved(float *in, float *coeff, size_t in_size, size_t coeff_size);

float *racs_allpass_coeff(float fc, float q, float fs, size_t *out_size);

float *racs_lowpass_coeff(float fc, float q, float fs, size_t *out_size);

float *racs_highpass_coeff(float fc, float q, float fs, size_t *out_size);

float *racs_bandpass_coeff(float fc, float bw, float fs, size_t *out_size);

float *racs_bandstop_coeff(float fc, float bw, float fs, size_t *out_size);

float *racs_lowshelf_coeff(float fc, float g, float fs, size_t *out_size);

float *racs_highshelf_coeff(float fc, float g, float fs, size_t *out_size);

float *racs_peaking_coeff(float fc, float q, float g, float fs, size_t *out_size);

#ifdef __cplusplus
}
#endif

#endif //RACS_BIQUAD_H
