
#ifndef RACS_DAW_OPS_H
#define RACS_DAW_OPS_H

#include "types.h"
#include <stdint.h>

racs_int32 *racs_daw_ops_mix(
    const racs_int32 *in_a,
    size_t in_a_len,
    const racs_int32 *in_b,
    size_t in_b_len,
    float gain_a,
    float gain_b,
    size_t *out_len
);

#endif //RACS_DAW_OPS_H