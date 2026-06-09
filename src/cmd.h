
#ifndef RACS_CMD_H
#define RACS_CMD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ctx.h"
#include "pack.h"
#include "stream.h"


typedef void (*racs_cmd_func) (racs_ctx *ctx, size_t num_args);

typedef struct {
    char          name[55];
    racs_cmd_func func;
    int           rd_only;
} racs_cmd;


extern const racs_cmd cmds[4];


int racs_cmd_arg_str(racs_ctx *ctx, char **arg, size_t *size);

int racs_cmd_arg_bin(racs_ctx *ctx, racs_uint8 **arg, size_t *size);

int racs_cmd_arg_uint64(racs_ctx *ctx, racs_uint64 *arg);

int racs_cmd_arg_uint32(racs_ctx *ctx, racs_uint32 *arg);

int racs_cmd_arg_uint16(racs_ctx *ctx, racs_uint16 *arg);

int racs_cmd_arg_uint8(racs_ctx *ctx, racs_uint8 *arg);

void racs_cmd_err(racs_ctx *ctx, const char *msg);

racs_cmd_func racs_cmd_lookup(const char *name);

void racs_cmd_ping(racs_ctx *ctx, size_t num_args);

void racs_cmd_create(racs_ctx *ctx, size_t num_args);

void racs_cmd_open(racs_ctx *ctx, size_t num_args);

void racs_cmd_stream(racs_ctx *ctx, size_t num_args);


#ifdef __cplusplus
}
#endif

#endif //RACS_CMD_H
