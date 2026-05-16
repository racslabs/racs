
#ifndef RACS_EVAL_H
#define RACS_EVAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"
#include "pack.h"
#include "unpack.h"
#include <ctype.h>


#define RACS_EXT_ERR 42

#define RACS_MAX_RECURSION_DEPTH 50


typedef struct {
    int             depth;
    int             has_error;
    racs_uint32     sample_rate;
    racs_uint8      channels;
    racs_uint8      bit_depth;
    msgpack_sbuffer out_buf;
} racs_eval_ctx;

typedef void (*racs_cmd_func) (racs_eval_ctx *ctx, size_t num_args);

typedef struct {
    char u_name[55];
    char l_name[55];
    racs_cmd_func func;
} racs_cmd;


const racs_cmd cmds[1] = {
    {"PING", "ping", NULL}
};

void racs_eval_ctx_init(racs_eval_ctx *ctx);

void racs_eval_ctx_cleanup(racs_eval_ctx *ctx);

void racs_eval_node(racs_eval_ctx *ctx, msgpack_object obj);

racs_cmd_func racs_cmd_lookup(const char *name, size_t size);


#ifdef __cplusplus
}
#endif

#endif //RACS_EVAL_H
