
#ifndef RACS_EVAL_H
#define RACS_EVAL_H

#ifdef __cplusplus
extern "C" {
#endif


#include "cmd.h"
#include <ctype.h>


#define RACS_MAX_RECURSION_DEPTH 100

void racs_eval(racs_ctx *ctx, const racs_uint8 *src, size_t size);


#ifdef __cplusplus
}
#endif

#endif //RACS_EVAL_H
