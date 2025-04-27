#include "exec.h"

rats_result rats_exec_stream(rats_context *ctx, rats_uint8 *data) {
    msgpack_sbuffer out_buf;
    msgpack_sbuffer_init(&out_buf);

    rats_stream(&out_buf, ctx, data);

    rats_result result;
    rats_result_init(&result, out_buf.size);
    memcpy(result.data, out_buf.data, out_buf.size);

    msgpack_sbuffer_destroy(&out_buf);
    return result;
}

rats_result rats_exec_exec(rats_exec *exec, rats_context *ctx, const char *cmd) {
    rats_parser parser;
    rats_parser_init(&parser, cmd);

    msgpack_sbuffer in_buf;
    msgpack_sbuffer out_buf;

    msgpack_sbuffer_init(&in_buf);
    msgpack_sbuffer_init(&out_buf);

    rats_exec_plan plan;
    rats_exec_plan_init(&plan);

    int status = rats_exec_plan_build(&plan, &out_buf, &parser);
    if (status != RATS_EXEC_STATUS_ABORT)
        rats_exec_plan_exec(&plan, exec, ctx, &in_buf, &out_buf);

    rats_exec_plan_destroy(&plan);

    rats_result result;
    rats_result_init(&result, out_buf.size);
    memcpy(result.data, out_buf.data, out_buf.size);

    msgpack_sbuffer_destroy(&in_buf);
    msgpack_sbuffer_destroy(&out_buf);

    return result;
}

int rats_exec_plan_build(rats_exec_plan *plan, msgpack_sbuffer *out_buf, rats_parser *parser) {
    rats_token prev;
    memset(&prev, 0, sizeof(rats_token));
    rats_token curr = rats_parser_next_token(parser);

    rats_command *cmd = NULL;
    int status = RATS_EXEC_STATUS_CONTINUE;

    while (curr.type != RATS_TOKEN_TYPE_EOF && status == RATS_EXEC_STATUS_CONTINUE) {
        switch (curr.type) {
            case RATS_TOKEN_TYPE_ID: {
                cmd = rats_handle_id(&curr, &prev);
                status = rats_command_handle_id(cmd, out_buf);
                break;
            }
            case RATS_TOKEN_TYPE_STR:
                status = rats_command_handle_str(cmd, out_buf, &curr, &prev);
                break;
            case RATS_TOKEN_TYPE_PIPE:
                rats_exec_plan_add_command(plan, cmd);
                break;
            case RATS_TOKEN_TYPE_INT:
                status = rats_command_handle_int64(cmd, out_buf, &curr, &prev);
                break;
            case RATS_TOKEN_TYPE_TIME:
                status = rats_command_handle_time(cmd, out_buf, &curr, &prev);
                break;
            case RATS_TOKEN_TYPE_FLOAT:
                status = rats_command_handle_float64(cmd, out_buf, &curr, &prev);
                break;
            case RATS_TOKEN_TYPE_ERROR: {
                rats_handle_error(curr.as.err.msg, out_buf);
                status = RATS_EXEC_STATUS_ABORT;
            }
            default:
                break;
        }

        prev = curr;
        curr = rats_parser_next_token(parser);
    }

    if (status != RATS_EXEC_STATUS_ABORT)
        rats_exec_plan_add_command(plan, cmd);

    return status;
}

void rats_exec_plan_exec(rats_exec_plan *plan, rats_exec *exec, rats_context *ctx, msgpack_sbuffer *in_buf,
                         msgpack_sbuffer *out_buf) {
    msgpack_packer pk;

    for (int i = 0; i < plan->num_cmd; ++i) {
        rats_command *cmd = plan->cmd[i];
        rats_command_func func = rats_exec_get(exec, cmd->name);
        if (!func) {
            rats_handle_unknown_command(cmd, out_buf);
            return;
        }
    }

    for (int i = 0; i < plan->num_cmd; ++i) {
        rats_command *cmd = plan->cmd[i];
        rats_command_func func = rats_exec_get(exec, cmd->name);

        msgpack_packer_init(&pk, in_buf, msgpack_sbuffer_write);
        rats_command_serialize_args(cmd, &pk);
        rats_status status = func(in_buf, out_buf, ctx);
        msgpack_sbuffer_clear(in_buf);

        if (status != RATS_STATUS_OK) break;
    }
}

rats_command_func rats_exec_get(rats_exec *exec, const char *cmd_name) {
    return rats_kvstore_get(exec->kv, (void *) cmd_name);
}

void rats_command_serialize_args(rats_command *cmd, msgpack_packer *pk) {
    msgpack_pack_array(pk, cmd->num_args);

    for (int i = 0; i < cmd->num_args; ++i) {
        rats_command_arg *arg = cmd->args[i];

        switch (arg->type) {
            case RATS_COMMAND_ARG_TYPE_STR:
                rats_command_arg_serialize_str(arg, pk);
                break;
            case RATS_COMMAND_ARG_TYPE_INT:
                rats_command_arg_serialize_int64(arg, pk);
                break;
            case RATS_COMMAND_ARG_TYPE_FLOAT:
                rats_command_arg_serialize_float64(arg, pk);
                break;
        }
    }
}

void rats_command_arg_serialize_str(rats_command_arg *arg, msgpack_packer *pk) {
    msgpack_pack_str_with_body(pk, arg->as.str.ptr, arg->as.str.size);
}

void rats_command_arg_serialize_int64(rats_command_arg *arg, msgpack_packer *pk) {
    msgpack_pack_int64(pk, arg->as.i64);
}

void rats_command_arg_serialize_float64(rats_command_arg *arg, msgpack_packer *pk) {
    msgpack_pack_double(pk, arg->as.f64);
}

rats_command *rats_handle_id(rats_token *curr, rats_token *prev) {
    if (prev->type == RATS_TOKEN_TYPE_PIPE)
        return rats_command_create(curr->as.id.ptr, RATS_COMMAND_OP_PIPE, curr->as.id.size + 1);

    if (prev->type == RATS_TOKEN_TYPE_NONE)
        return rats_command_create(curr->as.id.ptr, RATS_COMMAND_OP_NONE, curr->as.id.size + 1);

    return NULL;
}

int rats_command_handle_id(rats_command *cmd, msgpack_sbuffer *out_buf) {
    if (!cmd) {
        rats_handle_error("Token type 'id' is not a valid argument.", out_buf);
        return RATS_EXEC_STATUS_ABORT;
    }

    return RATS_EXEC_STATUS_CONTINUE;
}

int rats_command_handle_str(rats_command *cmd, msgpack_sbuffer *out_buf, rats_token *curr, rats_token *prev) {
    if (prev->type == RATS_TOKEN_TYPE_PIPE || prev->type == RATS_TOKEN_TYPE_NONE) {
        rats_handle_error("Token type 'string' is not a valid command.", out_buf);
        return RATS_EXEC_STATUS_ABORT;
    }

    rats_command_arg *arg = rats_command_arg_create_str(curr->as.str.ptr, curr->as.str.size);
    rats_command_add_arg(cmd, arg);

    return RATS_EXEC_STATUS_CONTINUE;
}

int rats_command_handle_time(rats_command *cmd, msgpack_sbuffer *out_buf, rats_token *curr, rats_token *prev) {
    if (prev->type == RATS_TOKEN_TYPE_PIPE || prev->type == RATS_TOKEN_TYPE_NONE) {
        rats_handle_error("Token type 'time' is not a valid command.", out_buf);
        return RATS_EXEC_STATUS_ABORT;
    }

    rats_command_arg *arg = rats_command_arg_create_int64(curr->as.time);
    rats_command_add_arg(cmd, arg);

    return RATS_EXEC_STATUS_CONTINUE;
}

int rats_command_handle_int64(rats_command *cmd, msgpack_sbuffer *out_buf, rats_token *curr, rats_token *prev) {
    if (prev->type == RATS_TOKEN_TYPE_PIPE || prev->type == RATS_TOKEN_TYPE_NONE) {
        rats_handle_error("Token type 'int' is not a valid command.", out_buf);
        return RATS_EXEC_STATUS_ABORT;
    }

    rats_command_arg *arg = rats_command_arg_create_int64(curr->as.i64);
    rats_command_add_arg(cmd, arg);

    return RATS_EXEC_STATUS_CONTINUE;
}

int rats_command_handle_float64(rats_command *cmd, msgpack_sbuffer *out_buf, rats_token *curr, rats_token *prev) {
    if (prev->type == RATS_TOKEN_TYPE_PIPE || prev->type == RATS_TOKEN_TYPE_NONE) {
        rats_handle_error("Token type 'float' is not a valid command.", out_buf);
        return RATS_EXEC_STATUS_ABORT;
    }

    rats_command_arg *arg = rats_command_arg_create_float64(curr->as.f64);
    rats_command_add_arg(cmd, arg);

    return RATS_EXEC_STATUS_CONTINUE;
}

void rats_exec_init(rats_exec *exec) {
    exec->kv = rats_kvstore_create(10, rats_exec_hash, rats_exec_cmp, exec_destroy);
    rats_kvstore_put(exec->kv, strdup("PING"), rats_command_ping);
    rats_kvstore_put(exec->kv, strdup("STREAM"), rats_command_stream);
    rats_kvstore_put(exec->kv, strdup("STREAMINFO"), rats_command_streaminfo);
    rats_kvstore_put(exec->kv, strdup("STREAMOPEN"), rats_command_streamopen);
    rats_kvstore_put(exec->kv, strdup("STREAMCLOSE"), rats_command_streamclose);
    rats_kvstore_put(exec->kv, strdup("EXTRACT"), rats_command_extract);
    rats_kvstore_put(exec->kv, strdup("EVAL"), rats_command_eval);
    rats_kvstore_put(exec->kv, strdup("FORMAT"), rats_command_format);
    rats_kvstore_put(exec->kv, strdup("SHUTDOWN"), rats_command_shutdown);
}

void rats_exec_destroy(rats_exec *exec) {
    rats_kvstore_destroy(exec->kv);
}

void rats_handle_error(const char *message, msgpack_sbuffer *out_buf) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);
    rats_serialize_error(&pk, message);
}

void rats_handle_unknown_command(rats_command *cmd, msgpack_sbuffer *out_buf) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    char *message = NULL;
    asprintf(&message, "Unknown command: %s", cmd->name);
    rats_serialize_error(&pk, message);

    free(message);
}

rats_command *rats_command_create(const char *name, rats_command_op op, size_t size) {
    rats_command *cmd = malloc(sizeof(rats_command));
    if (!cmd) {
        perror("Failed to allocate rats_command");
        return NULL;
    }

    cmd->num_args = 0;
    cmd->max_num_args = 4;
    cmd->op = op;

    strlcpy(cmd->name, name, size);
    rats_uppercase(cmd->name);

    cmd->args = malloc(cmd->max_num_args * sizeof(rats_command_arg *));
    if (!cmd->args) {
        perror("Failed to allocate args to rats_command");
        free(cmd);
        return NULL;
    }

    return cmd;
}

void rats_command_destroy(rats_command *cmd) {
    for (int i = 0; i < cmd->num_args; ++i) {
        rats_command_arg_destroy(cmd->args[i]);
    }

    free(cmd->args);
    free(cmd);
}

rats_command_arg *rats_command_arg_create() {
    rats_command_arg *arg = malloc(sizeof(rats_command_arg));
    if (!arg) {
        perror("Error allocating rats_command_arg");
        return NULL;
    }

    return arg;
}

rats_command_arg *rats_command_arg_create_str(const char *ptr, size_t size) {
    rats_command_arg *arg = rats_command_arg_create();
    if (!arg) return NULL;

    arg->type = RATS_COMMAND_ARG_TYPE_STR;
    arg->as.str.ptr = ptr;
    arg->as.str.size = size;

    return arg;
}

rats_command_arg *rats_command_arg_create_int64(rats_int64 d) {
    rats_command_arg *arg = rats_command_arg_create();
    if (!arg) return NULL;

    arg->type = RATS_COMMAND_ARG_TYPE_INT;
    arg->as.i64 = d;

    return arg;
}

rats_command_arg *rats_command_arg_create_float64(double d) {
    rats_command_arg *arg = rats_command_arg_create();
    if (!arg) return NULL;

    arg->type = RATS_COMMAND_ARG_TYPE_FLOAT;
    arg->as.f64 = d;

    return arg;
}

void rats_command_arg_destroy(rats_command_arg *arg) {
    free(arg);
}

void rats_command_add_arg(rats_command *cmd, rats_command_arg *arg) {
    if (!cmd || !arg) return;

    if (cmd->num_args >= cmd->max_num_args) {
        cmd->max_num_args *= 2;

        rats_command_arg **args = realloc(cmd->args, cmd->max_num_args * sizeof(rats_command_arg *));
        if (!args) {
            perror("Failed to reallocate args to rats_command");
            return;
        }

        cmd->args = args;
    }

    cmd->args[cmd->num_args] = arg;
    ++cmd->num_args;
}

void rats_exec_plan_init(rats_exec_plan *plan) {
    plan->num_cmd = 0;
    plan->max_num_cmd = 4;

    plan->cmd = malloc(plan->max_num_cmd * sizeof(rats_command *));
    if (!plan->cmd) {
        perror("Failed to allocate commands to rats_exec_plan");
        return;
    }
}

void rats_exec_plan_destroy(rats_exec_plan *plan) {
    for (int i = 0; i < plan->num_cmd; ++i) {
        rats_command_destroy(plan->cmd[i]);
    }
    free(plan->cmd);
}

void rats_exec_plan_add_command(rats_exec_plan *plan, rats_command *cmd) {
    if (!plan || !cmd) return;

    if (plan->num_cmd >= plan->max_num_cmd) {
        plan->max_num_cmd *= 2;

        rats_command **_cmd = realloc(plan->cmd, plan->max_num_cmd * sizeof(rats_command *));
        if (!_cmd) {
            perror("Error reallocating commands to rats_exec_plan");
            return;
        }

        plan->cmd = _cmd;
    }

    plan->cmd[plan->num_cmd] = cmd;
    ++plan->num_cmd;
}

rats_uint64 rats_exec_hash(void *key) {
    rats_uint64 hash[2];
    murmur3_x64_128(key, strlen((char *) key), 0, hash);
    return hash[0];
}

int rats_exec_cmp(void *a, void *b) {
    rats_uint64 *x = (rats_uint64 *) a;
    rats_uint64 *y = (rats_uint64 *) b;
    return x[0] == y[0];
}

void exec_destroy(void *key, void *value) {
    free(key);
}

void rats_uppercase(char *str) {
    while (*str) {
        *str = (char) toupper(*str);
        str++;
    }
}
