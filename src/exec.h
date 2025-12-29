// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_EXEC_H
#define RACS_EXEC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "kvstore.h"
#include "parser.h"
#include "command.h"

typedef enum {
    RACS_COMMAND_ARG_TYPE_STR,
    RACS_COMMAND_ARG_TYPE_INT,
    RACS_COMMAND_ARG_TYPE_FLOAT
} racs_command_arg_type;

typedef enum {
    RACS_COMMAND_OP_PIPE,
    RACS_COMMAND_OP_NONE
} racs_command_op;

typedef enum {
    RACS_EXEC_STATUS_CONTINUE,
    RACS_EXEC_STATUS_ABORT
} racs_exec_status;

typedef struct {
    racs_kvstore *kv;
} racs_exec;

typedef struct {
    const char *ptr;
    size_t size;
} racs_command_arg_;

typedef racs_command_arg_ racs_command_arg_str;

typedef union {
    double f64;
    racs_int64 i64;
    racs_uint64 u64;
    racs_command_arg_str str;
} racs_command_arg_union;

typedef struct {
    racs_command_arg_type type;
    racs_command_arg_union as;
} racs_command_arg;

typedef struct {
    char name[255];
    int num_args;
    int max_num_args;
    racs_command_op op;
    racs_command_arg **args;
} racs_command;

typedef struct {
    int num_cmd;
    int max_num_cmd;
    racs_command **cmd;
} racs_exec_plan;

typedef int (*racs_command_func)(msgpack_sbuffer *in_buf, msgpack_sbuffer *out_buf, racs_context *ctx);

void racs_exec_init(racs_exec *exec);

void racs_exec_destroy(racs_exec *exec);

racs_result racs_exec_exec(racs_exec *exec, racs_context *ctx, const char *cmd);

racs_result racs_exec_stream(racs_context *ctx, racs_uint8 *data);

racs_result racs_exec_stream_batch(racs_context *ctx, racs_uint8 *data, size_t size);

racs_uint64 racs_exec_hash(void *key);

int racs_exec_cmp(void *a, void *b);

void exec_destroy(void *key, void *value);

racs_command_func racs_exec_get(racs_exec *exec, const char *cmd_name);

racs_command *racs_command_create(const char *name, racs_command_op op, size_t size);

void racs_command_destroy(racs_command *cmd);

racs_command_arg *racs_command_arg_create();

racs_command_arg *racs_command_arg_create_str(const char *ptr, size_t size);

racs_command_arg *racs_command_arg_create_int64(racs_int64 d);

racs_command_arg *racs_command_arg_create_float64(double d);

void racs_command_arg_destroy(racs_command_arg *arg);

racs_command *racs_handle_id(racs_token *curr, racs_token *prev);

void racs_handle_error(const char *message, msgpack_sbuffer *out_buf);

void racs_handle_unknown_command(racs_command *cmd, msgpack_sbuffer *out_buf);

int racs_command_handle_id(racs_command *cmd, msgpack_sbuffer *out_buf);

int racs_command_handle_str(racs_command *cmd, msgpack_sbuffer *out_buf, racs_token *curr, racs_token *prev);

int racs_command_handle_int64(racs_command *cmd, msgpack_sbuffer *out_buf, racs_token *curr, racs_token *prev);

int racs_command_handle_float64(racs_command *cmd, msgpack_sbuffer *out_buf, racs_token *curr, racs_token *prev);

int racs_command_handle_time(racs_command *cmd, msgpack_sbuffer *out_buf, racs_token *curr, racs_token *prev);

void racs_command_add_arg(racs_command *cmd, racs_command_arg *arg);

void racs_command_serialize_args(racs_command *cmd, msgpack_packer *pk);

void racs_command_arg_serialize_str(racs_command_arg *arg, msgpack_packer *pk);

void racs_command_arg_serialize_int64(racs_command_arg *arg, msgpack_packer *pk);

void racs_command_arg_serialize_float64(racs_command_arg *arg, msgpack_packer *pk);

void racs_exec_plan_init(racs_exec_plan *plan);

void racs_exec_plan_destroy(racs_exec_plan *plan);

void racs_exec_plan_add_command(racs_exec_plan *plan, racs_command *cmd);

void racs_exec_plan_exec(racs_exec_plan *plan, racs_exec *exec, racs_context *ctx, msgpack_sbuffer *in_buf,
                         msgpack_sbuffer *out_buf);

int racs_exec_plan_build(racs_exec_plan *plan, msgpack_sbuffer *out_buf, racs_parser *parser);

void racs_uppercase(char *str);


#ifdef __cplusplus
}
#endif

#endif //RACS_EXEC_H
