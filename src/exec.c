// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "exec.h"

racs_result racs_exec_stream_batch(racs_context *ctx, racs_uint8 *data, size_t size) {
    msgpack_sbuffer out_buf;
    msgpack_sbuffer_init(&out_buf);

    racs_stream_batch(&out_buf, ctx, data + 4, size - 4);

    racs_result result;
    racs_result_init(&result, out_buf.size);
    memcpy(result.data, out_buf.data, out_buf.size);

    msgpack_sbuffer_destroy(&out_buf);
    return result;
}

racs_result racs_exec_stream(racs_context *ctx, racs_uint8 *data) {
    msgpack_sbuffer out_buf;
    msgpack_sbuffer_init(&out_buf);

    racs_stream(&out_buf, ctx, data);

    racs_result result;
    racs_result_init(&result, out_buf.size);
    memcpy(result.data, out_buf.data, out_buf.size);

    msgpack_sbuffer_destroy(&out_buf);
    return result;
}

racs_result racs_exec_exec(racs_exec *exec, racs_context *ctx, const char *cmd) {
    racs_parser parser;
    racs_parser_init(&parser, cmd);

    msgpack_sbuffer in_buf;
    msgpack_sbuffer out_buf;

    msgpack_sbuffer_init(&in_buf);
    msgpack_sbuffer_init(&out_buf);

    racs_exec_plan plan;
    racs_exec_plan_init(&plan);

    int rc = racs_exec_plan_build(&plan, &out_buf, &parser);
    if (rc != RACS_EXEC_STATUS_ABORT)
        racs_exec_plan_exec(&plan, exec, ctx, &in_buf, &out_buf);

    racs_exec_plan_destroy(&plan);

    racs_result result;
    racs_result_init(&result, out_buf.size);
    memcpy(result.data, out_buf.data, out_buf.size);

    msgpack_sbuffer_destroy(&in_buf);
    msgpack_sbuffer_destroy(&out_buf);

    return result;
}

int racs_exec_plan_build(racs_exec_plan *plan, msgpack_sbuffer *out_buf, racs_parser *parser) {
    racs_token prev;
    memset(&prev, 0, sizeof(racs_token));
    racs_token curr = racs_parser_next_token(parser);

    racs_command *cmd = NULL;
    int rc = RACS_EXEC_STATUS_CONTINUE;

    while (curr.type != RACS_TOKEN_TYPE_EOF && rc == RACS_EXEC_STATUS_CONTINUE) {
        switch (curr.type) {
            case RACS_TOKEN_TYPE_ID: {
                cmd = racs_handle_id(&curr, &prev);
                rc = racs_command_handle_id(cmd, out_buf);
                break;
            }
            case RACS_TOKEN_TYPE_STR:
                rc = racs_command_handle_str(cmd, out_buf, &curr, &prev);
                break;
            case RACS_TOKEN_TYPE_PIPE:
                racs_exec_plan_add_command(plan, cmd);
                break;
            case RACS_TOKEN_TYPE_INT:
                rc = racs_command_handle_int64(cmd, out_buf, &curr, &prev);
                break;
            case RACS_TOKEN_TYPE_TIME:
                rc = racs_command_handle_time(cmd, out_buf, &curr, &prev);
                break;
            case RACS_TOKEN_TYPE_FLOAT:
                rc = racs_command_handle_float64(cmd, out_buf, &curr, &prev);
                break;
            case RACS_TOKEN_TYPE_ERROR: {
                racs_handle_error(curr.as.err.msg, out_buf);
                rc = RACS_EXEC_STATUS_ABORT;
            }
            default:
                break;
        }

        prev = curr;
        curr = racs_parser_next_token(parser);
    }

    if (rc != RACS_EXEC_STATUS_ABORT)
        racs_exec_plan_add_command(plan, cmd);

    return rc;
}

void racs_exec_plan_exec(racs_exec_plan *plan, racs_exec *exec, racs_context *ctx, msgpack_sbuffer *in_buf,
                         msgpack_sbuffer *out_buf) {
    msgpack_packer pk;

    for (int i = 0; i < plan->num_cmd; ++i) {
        racs_command *cmd = plan->cmd[i];
        racs_command_func func = racs_exec_get(exec, cmd->name);
        if (!func) {
            racs_handle_unknown_command(cmd, out_buf);
            return;
        }
    }

    for (int i = 0; i < plan->num_cmd; ++i) {
        racs_command *cmd = plan->cmd[i];
        racs_command_func func = racs_exec_get(exec, cmd->name);

        msgpack_packer_init(&pk, in_buf, msgpack_sbuffer_write);
        racs_command_serialize_args(cmd, &pk);
        int rc = func(in_buf, out_buf, ctx);
        msgpack_sbuffer_clear(in_buf);

        if (rc != RACS_STATUS_OK) break;
    }
}

racs_command_func racs_exec_get(racs_exec *exec, const char *cmd_name) {
    return racs_kvstore_get(exec->kv, (void *) cmd_name);
}

void racs_command_serialize_args(racs_command *cmd, msgpack_packer *pk) {
    msgpack_pack_array(pk, cmd->num_args);

    for (int i = 0; i < cmd->num_args; ++i) {
        racs_command_arg *arg = cmd->args[i];

        switch (arg->type) {
            case RACS_COMMAND_ARG_TYPE_STR:
                racs_command_arg_serialize_str(arg, pk);
                break;
            case RACS_COMMAND_ARG_TYPE_INT:
                racs_command_arg_serialize_int64(arg, pk);
                break;
            case RACS_COMMAND_ARG_TYPE_FLOAT:
                racs_command_arg_serialize_float64(arg, pk);
                break;
        }
    }
}

void racs_command_arg_serialize_str(racs_command_arg *arg, msgpack_packer *pk) {
    msgpack_pack_str_with_body(pk, arg->as.str.ptr, arg->as.str.size);
}

void racs_command_arg_serialize_int64(racs_command_arg *arg, msgpack_packer *pk) {
    msgpack_pack_int64(pk, arg->as.i64);
}

void racs_command_arg_serialize_float64(racs_command_arg *arg, msgpack_packer *pk) {
    msgpack_pack_double(pk, arg->as.f64);
}

racs_command *racs_handle_id(racs_token *curr, racs_token *prev) {
    if (prev->type == RACS_TOKEN_TYPE_PIPE)
        return racs_command_create(curr->as.id.ptr, RACS_COMMAND_OP_PIPE, curr->as.id.size + 1);

    if (prev->type == RACS_TOKEN_TYPE_NONE)
        return racs_command_create(curr->as.id.ptr, RACS_COMMAND_OP_NONE, curr->as.id.size + 1);

    return NULL;
}

int racs_command_handle_id(racs_command *cmd, msgpack_sbuffer *out_buf) {
    if (!cmd) {
        racs_handle_error("Token type 'id' is not a valid argument.", out_buf);
        return RACS_EXEC_STATUS_ABORT;
    }

    return RACS_EXEC_STATUS_CONTINUE;
}

int racs_command_handle_str(racs_command *cmd, msgpack_sbuffer *out_buf, racs_token *curr, racs_token *prev) {
    if (prev->type == RACS_TOKEN_TYPE_PIPE || prev->type == RACS_TOKEN_TYPE_NONE) {
        racs_handle_error("Token type 'string' is not a valid command.", out_buf);
        return RACS_EXEC_STATUS_ABORT;
    }

    racs_command_arg *arg = racs_command_arg_create_str(curr->as.str.ptr, curr->as.str.size);
    racs_command_add_arg(cmd, arg);

    return RACS_EXEC_STATUS_CONTINUE;
}

int racs_command_handle_time(racs_command *cmd, msgpack_sbuffer *out_buf, racs_token *curr, racs_token *prev) {
    if (prev->type == RACS_TOKEN_TYPE_PIPE || prev->type == RACS_TOKEN_TYPE_NONE) {
        racs_handle_error("Token type 'time' is not a valid command.", out_buf);
        return RACS_EXEC_STATUS_ABORT;
    }

    racs_command_arg *arg = racs_command_arg_create_int64(curr->as.time);
    racs_command_add_arg(cmd, arg);

    return RACS_EXEC_STATUS_CONTINUE;
}

int racs_command_handle_int64(racs_command *cmd, msgpack_sbuffer *out_buf, racs_token *curr, racs_token *prev) {
    if (prev->type == RACS_TOKEN_TYPE_PIPE || prev->type == RACS_TOKEN_TYPE_NONE) {
        racs_handle_error("Token type 'int' is not a valid command.", out_buf);
        return RACS_EXEC_STATUS_ABORT;
    }

    racs_command_arg *arg = racs_command_arg_create_int64(curr->as.i64);
    racs_command_add_arg(cmd, arg);

    return RACS_EXEC_STATUS_CONTINUE;
}

int racs_command_handle_float64(racs_command *cmd, msgpack_sbuffer *out_buf, racs_token *curr, racs_token *prev) {
    if (prev->type == RACS_TOKEN_TYPE_PIPE || prev->type == RACS_TOKEN_TYPE_NONE) {
        racs_handle_error("Token type 'float' is not a valid command.", out_buf);
        return RACS_EXEC_STATUS_ABORT;
    }

    racs_command_arg *arg = racs_command_arg_create_float64(curr->as.f64);
    racs_command_add_arg(cmd, arg);

    return RACS_EXEC_STATUS_CONTINUE;
}

void racs_exec_init(racs_exec *exec) {
    exec->kv = racs_kvstore_create(10, racs_exec_hash, racs_exec_cmp, exec_destroy);
    racs_kvstore_put(exec->kv, strdup("PING"), racs_command_ping);
    racs_kvstore_put(exec->kv, strdup("CREATE"), racs_command_streamcreate);
    racs_kvstore_put(exec->kv, strdup("INFO"), racs_command_metadata);
    racs_kvstore_put(exec->kv, strdup("OPEN"), racs_command_streamopen);
    racs_kvstore_put(exec->kv, strdup("CLOSE"), racs_command_streamclose);
    racs_kvstore_put(exec->kv, strdup("SEARCH"), racs_command_streamlist);
    racs_kvstore_put(exec->kv, strdup("EXTRACT"), racs_command_extract);
    racs_kvstore_put(exec->kv, strdup("EVAL"), racs_command_eval);
    racs_kvstore_put(exec->kv, strdup("FORMAT"), racs_command_format);
    racs_kvstore_put(exec->kv, strdup("SHUTDOWN"), racs_command_shutdown);
}

void racs_exec_destroy(racs_exec *exec) {
    racs_kvstore_destroy(exec->kv);
}

void racs_handle_error(const char *message, msgpack_sbuffer *out_buf) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);
    racs_pack_error(&pk, message);
}

void racs_handle_unknown_command(racs_command *cmd, msgpack_sbuffer *out_buf) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    char *message = NULL;
    asprintf(&message, "Unknown command: %s", cmd->name);
    racs_pack_error(&pk, message);

    free(message);
}

racs_command *racs_command_create(const char *name, racs_command_op op, size_t size) {
    racs_command *cmd = malloc(sizeof(racs_command));
    if (!cmd) {
        racs_log_error("Failed to allocate racs_command");
        return NULL;
    }

    cmd->num_args = 0;
    cmd->max_num_args = 4;
    cmd->op = op;

    snprintf(cmd->name, size, "%s", name);
    racs_uppercase(cmd->name);

    cmd->args = malloc(cmd->max_num_args * sizeof(racs_command_arg *));
    if (!cmd->args) {
        racs_log_error("Failed to allocate args to racs_command");
        free(cmd);
        return NULL;
    }

    return cmd;
}

void racs_command_destroy(racs_command *cmd) {
    for (int i = 0; i < cmd->num_args; ++i) {
        racs_command_arg_destroy(cmd->args[i]);
    }

    free(cmd->args);
    free(cmd);
}

racs_command_arg *racs_command_arg_create() {
    racs_command_arg *arg = malloc(sizeof(racs_command_arg));
    if (!arg) {
        racs_log_error("Error allocating racs_command_arg");
        return NULL;
    }

    return arg;
}

racs_command_arg *racs_command_arg_create_str(const char *ptr, size_t size) {
    racs_command_arg *arg = racs_command_arg_create();
    if (!arg) return NULL;

    arg->type = RACS_COMMAND_ARG_TYPE_STR;
    arg->as.str.ptr = ptr;
    arg->as.str.size = size;

    return arg;
}

racs_command_arg *racs_command_arg_create_int64(racs_int64 d) {
    racs_command_arg *arg = racs_command_arg_create();
    if (!arg) return NULL;

    arg->type = RACS_COMMAND_ARG_TYPE_INT;
    arg->as.i64 = d;

    return arg;
}

racs_command_arg *racs_command_arg_create_float64(double d) {
    racs_command_arg *arg = racs_command_arg_create();
    if (!arg) return NULL;

    arg->type = RACS_COMMAND_ARG_TYPE_FLOAT;
    arg->as.f64 = d;

    return arg;
}

void racs_command_arg_destroy(racs_command_arg *arg) {
    free(arg);
}

void racs_command_add_arg(racs_command *cmd, racs_command_arg *arg) {
    if (!cmd || !arg) return;

    if (cmd->num_args >= cmd->max_num_args) {
        cmd->max_num_args *= 2;

        racs_command_arg **args = realloc(cmd->args, cmd->max_num_args * sizeof(racs_command_arg *));
        if (!args) {
            racs_log_error("Failed to reallocate args to racs_command");
            return;
        }

        cmd->args = args;
    }

    cmd->args[cmd->num_args] = arg;
    ++cmd->num_args;
}

void racs_exec_plan_init(racs_exec_plan *plan) {
    plan->num_cmd = 0;
    plan->max_num_cmd = 4;

    plan->cmd = malloc(plan->max_num_cmd * sizeof(racs_command *));
    if (!plan->cmd) {
        racs_log_error("Failed to allocate commands to racs_exec_plan");
        return;
    }
}

void racs_exec_plan_destroy(racs_exec_plan *plan) {
    for (int i = 0; i < plan->num_cmd; ++i) {
        racs_command_destroy(plan->cmd[i]);
    }
    free(plan->cmd);
}

void racs_exec_plan_add_command(racs_exec_plan *plan, racs_command *cmd) {
    if (!plan || !cmd) return;

    if (plan->num_cmd >= plan->max_num_cmd) {
        plan->max_num_cmd *= 2;

        racs_command **_cmd = realloc(plan->cmd, plan->max_num_cmd * sizeof(racs_command *));
        if (!_cmd) {
            racs_log_error("Error reallocating commands to racs_exec_plan");
            return;
        }

        plan->cmd = _cmd;
    }

    plan->cmd[plan->num_cmd] = cmd;
    ++plan->num_cmd;
}

racs_uint64 racs_exec_hash(void *key) {
    racs_uint64 hash[2];
    murmur3_x64_128(key, strlen((char *) key), 0, hash);
    return hash[0];
}

int racs_exec_cmp(void *a, void *b) {
    racs_uint64 x = racs_exec_hash(a);
    racs_uint64 y = racs_exec_hash(b);
    return x == y;
}

void exec_destroy(void *key, void *value) {
    free(key);
}

void racs_uppercase(char *str) {
    while (*str) {
        *str = (char) toupper(*str);
        str++;
    }
}
