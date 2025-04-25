
#ifndef AUXTS_PCM_SIMD_H
#define AUXTS_PCM_SIMD_H

#include "simd.h"

void rats_simd_interleave_s16(const rats_int16* q0, const rats_int16* q1, rats_int16* q2, size_t n);
void rats_simd_planar_s16(const rats_int16* q0, rats_int16* q1, size_t n);

#endif //AUXTS_PCM_SIMD_H
