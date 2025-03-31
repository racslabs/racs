

#ifndef AUXTS_PCM_SIMD_H
#define AUXTS_PCM_SIMD_H

#include "simd.h"

void auxts_simd_interleave_s32(auxts_int32* q0, auxts_int32* q1, auxts_int32* q2, size_t n);
void auxts_simd_interleave_s16(auxts_int16* q0, auxts_int16* q1, auxts_int16* q2, size_t n);
void auxts_simd_extract_s16(auxts_int32* q0, auxts_int16* q1, size_t n);

#endif //AUXTS_PCM_SIMD_H
