
#ifndef RACS_DAW_OPS_H
#define RACS_DAW_OPS_H

#include "types.h"
#include "log.h"
#include <stdint.h>

racs_int32 *racs_daw_ops_mix(
    const racs_int32 *in_a,
    size_t in_a_len,
    const racs_int32 *in_b,
    size_t in_b_len,
    size_t *out_len
);

racs_int32 *racs_daw_ops_gain(
    const racs_int32 *in,
    size_t in_len,
    double gain
);

racs_int32 *racs_daw_ops_trim(
    const racs_int32 *in,
    size_t in_len,
    double left_seconds,
    double right_seconds,
    size_t *out_len
);

racs_int32 *racs_daw_ops_fade(
    const racs_int32 *in,
    size_t in_len,
    double fade_in_seconds,
    double fade_out_seconds,
    size_t *out_len
);

racs_int32 *racs_daw_ops_pan(
    const racs_int32 *in,
    size_t in_len,
    double pan,
    size_t *out_len
);

racs_int32 *racs_daw_ops_pad(
    const racs_int32 *in,
    size_t in_len,
    double left_seconds,
    double right_seconds,
    size_t *out_len
);

racs_int32 *racs_daw_ops_clip(
    const racs_int32 *in,
    size_t in_len,
    racs_int32 min_val,
    racs_int32 max_val,
    size_t *out_len
);

#endif //RACS_DAW_OPS_H