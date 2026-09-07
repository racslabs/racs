
#include "simd.h"


void racs_simd_interleave_int24(const racs_int24 *src_l, const racs_int24 *src_r, racs_int24 *dst, size_t n) {
    if (!src_l || !src_r || !dst || n == 0) {
        return;
    }

    size_t i = 0;
    
    RACS_UNROLL
    for ( ; i < (n & ~3); i++) {
        dst[i * 2]       = src_l[i]; 
        dst[(i * 2) + 1] = src_r[i];
    }

    for ( ; i < n; i++) {
        dst[i * 2]       = src_l[i];
        dst[(i * 2) + 1] = src_r[i];
    }
}
