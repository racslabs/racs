
#ifndef RACS_EVAL_H
#define RACS_EVAL_H

#ifdef __cplusplus
extern "C" {
#endif


#include "create.h"
#include "config.h"
#include "types.h"
#include "pack.h"
#include <ctype.h>


#define RACS_EXT_ERR 42

#define RACS_MAX_RECURSION_DEPTH 100


typedef struct {
    int             depth;
    int             has_error;
    racs_uint32     sample_rate;
    racs_uint8      channels;
    racs_uint8      bit_depth;
    msgpack_sbuffer out_buf;
} racs_eval_ctx;


void racs_eval_ctx_init(racs_eval_ctx *ctx);

void racs_eval_ctx_cleanup(racs_eval_ctx *ctx);

void racs_eval(racs_eval_ctx *ctx, const racs_uint8 *source, size_t size);


#ifdef __cplusplus
}
#endif

#endif //RACS_EVAL_H
