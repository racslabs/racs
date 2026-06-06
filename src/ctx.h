
#ifndef RACS_CTX_H
#define RACS_CTX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include <msgpack.h>

typedef struct {
    int             depth;
    int             has_error;
    size_t          offset;
    racs_uint32     sample_rate;
    racs_uint8      channels;
    racs_uint8      bit_depth;
    msgpack_sbuffer out_buf;
} racs_ctx;


void racs_ctx_init(racs_ctx *ctx);

void racs_ctx_cleanup(racs_ctx *ctx);


#ifdef __cplusplus
}
#endif

#endif //RACS_CTX_H
