
#ifndef AUXTS_PCM_SIMD_H
#define AUXTS_PCM_SIMD_H

#include "simd.h"

void auxts_simd_interleave_s16(const auxts_int16* q0, const auxts_int16* q1, auxts_int16* q2, size_t n);
void auxts_simd_planar_s16(const auxts_int16* q0, auxts_int16* q1, size_t n);

#endif //AUXTS_PCM_SIMD_H
