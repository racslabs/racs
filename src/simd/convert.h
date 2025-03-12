
#ifndef AUXTS_CONVERT_H
#define AUXTS_CONVERT_H

#include "simd.h"

void simd_convert_float32_to_int32(float* a, int32_t* b, size_t n);
void simd_convert_int32_to_float32(int32_t* a, float* b, size_t n);

#endif //AUXTS_CONVERT_H
