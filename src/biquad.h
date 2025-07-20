
#ifndef RACS_BIQUAD_H
#define RACS_BIQUAD_H

#include <string.h>
#include "types.h"
#include "capi/biquad.h"

racs_int16 *racs_biquad_s16(racs_int16 *in,
                            const char *type,
                            float fc,
                            float fs,
                            float p0,
                            racs_uint16 channels,
                            size_t size);


#endif //RACS_BIQUAD_H
