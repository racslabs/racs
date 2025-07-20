
#include "biquad.h"

using namespace racs;
using namespace racs::signal;

float *racs_biquad(float *in, float *coeff, size_t in_size, size_t coeff_size) {
    tensor<1, float> _in(tensor_shape({ in_size }), in);
    tensor<1, float> _coeff(tensor_shape({ coeff_size }), coeff);

    auto _out = biquad(_in, _coeff);
    return _out.detach();
}

float *racs_biquad_interleaved(float *in, float *coeff, size_t in_size, size_t coeff_size) {
    tensor<1, float> _in(tensor_shape({ in_size * 2 }), in);
    tensor<1, float> _coeff(tensor_shape({ coeff_size }), coeff);

    tensor<2, float> p_in = planar(_in);
    auto _out = biquad(p_in, _coeff);
    auto i_out = interleaved(_out);

    return i_out.detach();
}

float *racs_allpass_coeff(float fc, float q, float fs, size_t *out_size) {
    auto _out = allpass(fc, q, fs);
    *out_size = _out.size();
    return _out.detach();
}

float *racs_lowpass_coeff(float fc, float q, float fs, size_t *out_size) {
    auto _out = lowpass(fc, q, fs);
    *out_size = _out.size();
    return _out.detach();
}

float *racs_highpass_coeff(float fc, float q, float fs, size_t *out_size) {
    auto _out = highpass(fc, q, fs);
    *out_size = _out.size();
    return _out.detach();
}

float *racs_bandpass_coeff(float fc, float bw, float fs, size_t *out_size) {
    auto _out = bandpass(fc, bw, fs);
    *out_size = _out.size();
    return _out.detach();
}

float *racs_bandstop_coeff(float fc, float bw, float fs, size_t *out_size) {
    auto _out = bandstop(fc, bw, fs);
    *out_size = _out.size();
    return _out.detach();
}

float *racs_lowshelf_coeff(float fc, float g, float fs, size_t *out_size) {
    auto _out = lowshelf(fc, g, fs);
    *out_size = _out.size();
    return _out.detach();
}

float *racs_highshelf_coeff(float fc, float g, float fs, size_t *out_size) {
    auto _out = highshelf(fc, g, fs);
    *out_size = _out.size();
    return _out.detach();
}

float *racs_peaking_coeff(float fc, float q, float g, float fs, size_t *out_size) {
    auto _out = peaking(fc, q, g, fs);
    *out_size = _out.size();
    return _out.detach();
}
