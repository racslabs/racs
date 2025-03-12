
#ifndef AUXTS_MERGE_H
#define AUXTS_MERGE_H

#include "merge_neon.h"

void simd_merge(float* a, float* b, float* c, size_t n, int m);

#endif //AUXTS_MERGE_H
