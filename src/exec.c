#include "exec.h"

static uint64_t exec_hash(void* key);
static int exec_cmp(void* a, void* b);
static void exec_destroy(void* key, void* value);
static rats_command_func exec_get(rats_exec* exec, const char* cmd_name);
static rats_command* command_create(const char* name, rats_command_op op, size_t size);
static void command_destroy(rats_command* cmd);
static rats_command_arg* command_arg_create();
static rats_command_arg* command_arg_create_str(const char* ptr, size_t size);
static rats_command_arg* command_arg_create_int64(int64_t d);
static rats_command_arg* command_arg_create_float64(double d);
static void command_arg_destroy(rats_command_arg* arg);
static rats_command* handle_id(rats_token* curr, rats_token* prev);
static void handle_error(const char* message, msgpack_sbuffer* out_buf);
static void handle_unknown_command(rats_command* cmd, msgpack_sbuffer* out_buf);
static int command_handle_id(rats_command* cmd, msgpack_sbuffer* out_buf);
static int command_handle_str(rats_command* cmd, msgpack_sbuffer* out_buf, rats_token* curr, rats_token* prev);
static int command_handle_int64(rats_command* cmd, msgpack_sbuffer* out_buf, rats_token* curr, rats_token* prev);
static int command_handle_float64(rats_command* cmd, msgpack_sbuffer* out_buf, rats_token* curr, rats_token* prev);
static int command_handle_time(rats_command* cmd, msgpack_sbuffer* out_buf, rats_token* curr, rats_token* prev);
static void command_add_arg(rats_command* cmd, rats_command_arg* arg);
static void command_serialize_args(rats_command* cmd, msgpack_packer* pk);
static void command_arg_serialize_str(rats_command_arg* arg, msgpack_packer* pk);
static void command_arg_serialize_int64(rats_command_arg* arg, msgpack_packer* pk);
static void command_arg_serialize_float64(rats_command_arg* arg, msgpack_packer* pk);
static void exec_plan_init(rats_exec_plan* plan);
static void exec_plan_destroy(rats_exec_plan* plan);
static void exec_plan_add_command(rats_exec_plan* plan, rats_command* cmd);
static void exec_plan_exec(rats_exec_plan* plan, rats_exec* exec, rats_context* ctx, msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf);
static int exec_plan_build(rats_exec_plan* plan, msgpack_sbuffer* out_buf, rats_parser* parser);
static void to_uppercase(char *str);

rats_result rats_exec_stream(rats_context* ctx, rats_uint8* data) {
    msgpack_sbuffer out_buf;
    msgpack_sbuffer_init(&out_buf);

    rats_stream(&out_buf, ctx, data);

    rats_result result;
    rats_result_init(&result, out_buf.size);
    memcpy(result.data, out_buf.data, out_buf.size);

    msgpack_sbuffer_destroy(&out_buf);
    return result;
}

rats_result rats_exec_exec(rats_exec* exec, rats_context* ctx, const char* cmd) {
    rats_parser parser;
    rats_parser_init(&parser, cmd);

    msgpack_sbuffer in_buf;
    msgpack_sbuffer out_buf;

    msgpack_sbuffer_init(&in_buf);
    msgpack_sbuffer_init(&out_buf);

    rats_exec_plan plan;
    exec_plan_init(&plan);

    int status = exec_plan_build(&plan, &out_buf, &parser);
    if (status != RATS_EXEC_STATUS_ABORT)
        exec_plan_exec(&plan, exec, ctx, &in_buf, &out_buf);

    exec_plan_destroy(&plan);

    rats_result result;
    rats_result_init(&result, out_buf.size);
    memcpy(result.data, out_buf.data, out_buf.size);

    msgpack_sbuffer_destroy(&in_buf);
    msgpack_sbuffer_destroy(&out_buf);

    return result;
}

int exec_plan_build(rats_exec_plan* plan, msgpack_sbuffer* out_buf, rats_parser* parser) {
    rats_token prev;
    memset(&prev, 0, sizeof(rats_token));
    rats_token curr = rats_parser_next_token(parser);

    rats_command* cmd = NULL;
    int status = RATS_EXEC_STATUS_CONTINUE;

    while (curr.type != RATS_TOKEN_TYPE_EOF && status == RATS_EXEC_STATUS_CONTINUE) {
        switch (curr.type) {
            case RATS_TOKEN_TYPE_ID: {
                cmd = handle_id(&curr, &prev);
                status = command_handle_id(cmd, out_buf);
                break;
            }
            case RATS_TOKEN_TYPE_STR:
                status = command_handle_str(cmd, out_buf, &curr, &prev);
                break;
            case RATS_TOKEN_TYPE_PIPE:
                exec_plan_add_command(plan, cmd);
                break;
            case RATS_TOKEN_TYPE_INT:
                status = command_handle_int64(cmd, out_buf, &curr, &prev);
                break;
            case RATS_TOKEN_TYPE_TIME:
                status = command_handle_time(cmd, out_buf, &curr, &prev);
                break;
            case RATS_TOKEN_TYPE_FLOAT:
                status = command_handle_float64(cmd, out_buf, &curr, &prev);
                break;
            case RATS_TOKEN_TYPE_ERROR: {
                handle_error(curr.as.err.msg, out_buf);
                status = RATS_EXEC_STATUS_ABORT;
            }
            default:
                break;
        }

        prev = curr;
        curr = rats_parser_next_token(parser);
    }

    if (status != RATS_EXEC_STATUS_ABORT)
        exec_plan_add_command(plan, cmd);

    return status;
}

void exec_plan_exec(rats_exec_plan* plan, rats_exec* exec, rats_context* ctx, msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf) {
    msgpack_packer pk;

    for (int i = 0; i < plan->num_cmd; ++i) {
        msgpack_packer_init(&pk, in_buf, msgpack_sbuffer_write);

        rats_command* cmd = plan->cmd[i];
        rats_command_func func = exec_get(exec, cmd->name);
        if (!func) {
            handle_unknown_command(cmd, out_buf);
            break;
        }

        command_serialize_args(cmd, &pk);
        rats_status status = func(in_buf, out_buf, ctx);
        msgpack_sbuffer_clear(in_buf);

        if (status != RATS_STATUS_OK) break;
    }
}

rats_command_func exec_get(rats_exec* exec, const char* cmd_name) {
    return rats_kvstore_get(exec->kv, (void*)cmd_name);
}

void command_serialize_args(rats_command* cmd, msgpack_packer* pk) {
    msgpack_pack_array(pk, cmd->num_args);

    for (int i = 0; i < cmd->num_args; ++i) {
        rats_command_arg* arg = cmd->args[i];

        switch (arg->type) {
            case RATS_COMMAND_ARG_TYPE_STR:
                command_arg_serialize_str(arg, pk);
                break;
            case RATS_COMMAND_ARG_TYPE_INT:
                command_arg_serialize_int64(arg, pk);
                break;
            case RATS_COMMAND_ARG_TYPE_FLOAT:
                command_arg_serialize_float64(arg, pk);
                break;
        }
    }
}

void command_arg_serialize_str(rats_command_arg* arg, msgpack_packer* pk) {
    msgpack_pack_str_with_body(pk, arg->as.str.ptr, arg->as.str.size);
}

void command_arg_serialize_int64(rats_command_arg* arg, msgpack_packer* pk) {
    msgpack_pack_int64(pk, arg->as.i64);
}

void command_arg_serialize_float64(rats_command_arg* arg, msgpack_packer* pk) {
    msgpack_pack_double(pk, arg->as.f64);
}

rats_command* handle_id(rats_token* curr, rats_token* prev) {
    if (prev->type == RATS_TOKEN_TYPE_PIPE)
        return command_create(curr->as.id.ptr, RATS_COMMAND_OP_PIPE, curr->as.id.size + 1);

    if (prev->type == RATS_TOKEN_TYPE_NONE)
        return command_create(curr->as.id.ptr, RATS_COMMAND_OP_NONE, curr->as.id.size + 1);

    return NULL;
}

int command_handle_id(rats_command* cmd, msgpack_sbuffer* out_buf) {
    if (!cmd) {
        handle_error("Token type 'id' is not a valid argument.", out_buf);
        return RATS_EXEC_STATUS_ABORT;
    }

    return RATS_EXEC_STATUS_CONTINUE;
}

int command_handle_str(rats_command* cmd, msgpack_sbuffer* out_buf, rats_token* curr, rats_token* prev) {
    if (prev->type == RATS_TOKEN_TYPE_PIPE || prev->type == RATS_TOKEN_TYPE_NONE) {
        handle_error("Token type 'string' is not a valid command.", out_buf);
        return RATS_EXEC_STATUS_ABORT;
    }

    rats_command_arg* arg = command_arg_create_str(curr->as.str.ptr, curr->as.str.size);
    command_add_arg(cmd, arg);

    return RATS_EXEC_STATUS_CONTINUE;
}

int command_handle_time(rats_command* cmd, msgpack_sbuffer* out_buf, rats_token* curr, rats_token* prev) {
    if (prev->type == RATS_TOKEN_TYPE_PIPE || prev->type == RATS_TOKEN_TYPE_NONE) {
        handle_error("Token type 'time' is not a valid command.", out_buf);
        return RATS_EXEC_STATUS_ABORT;
    }

    rats_command_arg* arg = command_arg_create_int64(curr->as.time);
    command_add_arg(cmd, arg);

    return RATS_EXEC_STATUS_CONTINUE;
}

int command_handle_int64(rats_command* cmd, msgpack_sbuffer* out_buf, rats_token* curr, rats_token* prev) {
    if (prev->type == RATS_TOKEN_TYPE_PIPE || prev->type == RATS_TOKEN_TYPE_NONE) {
        handle_error("Token type 'int' is not a valid command.", out_buf);
        return RATS_EXEC_STATUS_ABORT;
    }

    rats_command_arg* arg = command_arg_create_int64(curr->as.i64);
    command_add_arg(cmd, arg);

    return RATS_EXEC_STATUS_CONTINUE;
}

int command_handle_float64(rats_command* cmd, msgpack_sbuffer* out_buf, rats_token* curr, rats_token* prev) {
    if (prev->type == RATS_TOKEN_TYPE_PIPE || prev->type == RATS_TOKEN_TYPE_NONE) {
        handle_error("Token type 'float' is not a valid command.", out_buf);
        return RATS_EXEC_STATUS_ABORT;
    }

    rats_command_arg* arg = command_arg_create_float64(curr->as.f64);
    command_add_arg(cmd, arg);

    return RATS_EXEC_STATUS_CONTINUE;
}

void rats_exec_init(rats_exec* exec) {
    exec->kv = rats_kvstore_create(10, exec_hash, exec_cmp, exec_destroy);
    rats_kvstore_put(exec->kv, strdup("PING"), rats_command_ping);
    rats_kvstore_put(exec->kv, strdup("STREAM"), rats_command_stream);
    rats_kvstore_put(exec->kv, strdup("STREAMINFO"), rats_command_streaminfo);
    rats_kvstore_put(exec->kv, strdup("STREAMOPEN"), rats_command_streamopen);
    rats_kvstore_put(exec->kv, strdup("STREAMCLOSE"), rats_command_streamclose);
    rats_kvstore_put(exec->kv, strdup("EXTRACT"), rats_command_extract);
    rats_kvstore_put(exec->kv, strdup("EVAL"), rats_command_eval);
    rats_kvstore_put(exec->kv, strdup("FORMAT"), rats_command_format);
}

void rats_exec_destroy(rats_exec* exec) {
    rats_kvstore_destroy(exec->kv);
}

void handle_error(const char* message, msgpack_sbuffer* out_buf) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);
    rats_serialize_error(&pk, message);
}

void handle_unknown_command(rats_command* cmd, msgpack_sbuffer* out_buf) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    char* message = NULL;
    asprintf(&message, "Unknown command: %s", cmd->name);
    rats_serialize_error(&pk, message);

    free(message);
}

rats_command* command_create(const char* name, rats_command_op op, size_t size) {
    rats_command* cmd = malloc(sizeof(rats_command));
    if (!cmd) {
        perror("Failed to allocate rats_command");
        return NULL;
    }

    cmd->num_args = 0;
    cmd->max_num_args = 4;
    cmd->op = op;

    strlcpy(cmd->name, name, size);
    to_uppercase(cmd->name);

    cmd->args = malloc(cmd->max_num_args * sizeof(rats_command_arg*));
    if (!cmd->args) {
        perror("Failed to allocate args to rats_command");
        free(cmd);
        return NULL;
    }

    return cmd;
}

void command_destroy(rats_command* cmd) {
    for (int i = 0; i < cmd->num_args; ++i) {
        command_arg_destroy(cmd->args[i]);
    }

    free(cmd->args);
    free(cmd);
}

rats_command_arg* command_arg_create() {
    rats_command_arg* arg = malloc(sizeof(rats_command_arg));
    if (!arg) {
        perror("Error allocating rats_command_arg");
        return NULL;
    }

    return arg;
}

rats_command_arg* command_arg_create_str(const char* ptr, size_t size) {
    rats_command_arg* arg = command_arg_create();
    if (!arg) return NULL;

    arg->type = RATS_COMMAND_ARG_TYPE_STR;
    arg->as.str.ptr = ptr;
    arg->as.str.size = size;

    return arg;
}

rats_command_arg* command_arg_create_int64(int64_t d) {
    rats_command_arg* arg = command_arg_create();
    if (!arg) return NULL;

    arg->type = RATS_COMMAND_ARG_TYPE_INT;
    arg->as.i64 = d;

    return arg;
}

rats_command_arg* command_arg_create_float64(double d) {
    rats_command_arg* arg = command_arg_create();
    if (!arg) return NULL;

    arg->type = RATS_COMMAND_ARG_TYPE_FLOAT;
    arg->as.f64 = d;

    return arg;
}

void command_arg_destroy(rats_command_arg* arg) {
    free(arg);
}

void command_add_arg(rats_command* cmd, rats_command_arg* arg) {
    if (!cmd || !arg) return;

    if (cmd->num_args >= cmd->max_num_args) {
        cmd->max_num_args *= 2;

        rats_command_arg** args = realloc(cmd->args, cmd->max_num_args * sizeof(rats_command_arg*));
        if (!args) {
            perror("Failed to reallocate args to rats_command");
            return;
        }

        cmd->args = args;
    }

    cmd->args[cmd->num_args] = arg;
    ++cmd->num_args;
}

void exec_plan_init(rats_exec_plan* plan) {
    plan->num_cmd = 0;
    plan->max_num_cmd = 4;

    plan->cmd = malloc(plan->max_num_cmd * sizeof(rats_command*));
    if (!plan->cmd) {
        perror("Failed to allocate commands to rats_exec_plan");
        return;
    }
}

void exec_plan_destroy(rats_exec_plan* plan) {
    for (int i = 0; i < plan->num_cmd; ++i) {
        command_destroy(plan->cmd[i]);
    }
    free(plan->cmd);
}

void exec_plan_add_command(rats_exec_plan* plan, rats_command* cmd) {
    if (!plan || !cmd) return;

    if (plan->num_cmd >= plan->max_num_cmd) {
        plan->max_num_cmd *= 2;

        rats_command** _cmd = realloc(plan->cmd, plan->max_num_cmd * sizeof(rats_command*));
        if (!_cmd) {
            perror("Error reallocating commands to rats_exec_plan");
            return;
        }

        plan->cmd = _cmd;
    }

    plan->cmd[plan->num_cmd] = cmd;
    ++plan->num_cmd;
}

uint64_t exec_hash(void* key) {
    uint64_t hash[2];
    murmur3_x64_128(key, strlen((char*)key), 0, hash);
    return hash[0];
}

int exec_cmp(void* a, void* b) {
    uint64_t* x = (uint64_t*)a;
    uint64_t* y = (uint64_t*)b;
    return x[0] == y[0];
}

void exec_destroy(void* key, void* value) {
    free(key);
}

void to_uppercase(char *str) {
    while (*str) {
        *str = (char)toupper(*str);
        str++;
    }
}
