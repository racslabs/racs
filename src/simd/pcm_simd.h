
#ifndef RACS_PCM_SIMD_H
#define RACS_PCM_SIMD_H

#include "simd.h"

void racs_simd_interleave_s16(const racs_int16 *q0, const racs_int16 *q1, racs_int16 *q2, size_t n);

void racs_simd_planar_s16(const racs_int16 *q0, racs_int16 *q1, size_t n);

#endif //RACS_PCM_SIMD_H
