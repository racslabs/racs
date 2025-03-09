#include "command_executor.h"

static uint64_t command_executor_hash(void* key);
static int command_executor_cmp(void* a, void* b);
static void command_executor_destroy(void* key, void* value);
static auxts_command_func command_executor_get(auxts_command_executor* exec, const char* cmd_name);
static auxts_command* command_create(const char* name, size_t size);
static void command_destroy(auxts_command* cmd);
static auxts_command_arg* command_arg_create();
static auxts_command_arg* command_arg_create_str(const char* ptr, size_t size);
static auxts_command_arg* command_arg_create_bin(const char* ptr, size_t size);
static auxts_command_arg* command_arg_create_int32(int32_t d);
static auxts_command_arg* command_arg_create_float32(float d);
static void command_arg_destroy(auxts_command_arg* arg);
static auxts_command* command_handle_id(auxts_token* token);
static auxts_result handle_error(const char* message, msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf);
static void handle_unknown_command(auxts_command* cmd, msgpack_sbuffer* out_buf);
static void command_handle_str(auxts_command* cmd, auxts_token* token);
static void command_handle_bin(auxts_command* cmd, auxts_token* token);
static void command_handle_int32(auxts_command* cmd, auxts_token* token);
static void command_handle_float32(auxts_command* cmd, auxts_token* token);
static void command_add_arg(auxts_command* cmd, auxts_command_arg* arg);
static void command_serialize_args(auxts_command* cmd, msgpack_packer* pk);
static void command_arg_serialize_str(auxts_command_arg* arg, msgpack_packer* pk);
static void command_arg_serialize_bin(auxts_command_arg* arg, msgpack_packer* pk);
static void command_arg_serialize_int32(auxts_command_arg* arg, msgpack_packer* pk);
static void command_arg_serialize_float32(auxts_command_arg* arg, msgpack_packer* pk);
static void command_execution_plan_init(auxts_command_execution_plan* plan);
static void command_execution_plan_destroy(auxts_command_execution_plan* plan);
static void command_execution_plan_add_command(auxts_command_execution_plan* plan, auxts_command* cmd);
static void command_execution_plan_execute(auxts_command_execution_plan* plan, auxts_command_executor* exec, auxts_context* ctx, msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf);
static void to_uppercase(char *str);

auxts_result auxts_command_executor_execute(auxts_command_executor* exec, auxts_context* ctx, const char* cmd) {
    auxts_result result;

    auxts_parser parser;
    auxts_parser_init(&parser, cmd);

    msgpack_sbuffer in_buf;
    msgpack_sbuffer out_buf;

    msgpack_sbuffer_init(&in_buf);
    msgpack_sbuffer_init(&out_buf);

    auxts_token token = auxts_parser_next_token(&parser);
    if (token.type != AUXTS_TOKEN_TYPE_ID) {
        return handle_error("Invalid token at 0", &in_buf, &out_buf);
    }

    auxts_command_execution_plan plan;
    command_execution_plan_init(&plan);

    auxts_command* _cmd = NULL;
    while (token.type != AUXTS_TOKEN_TYPE_EOF) {
        switch (token.type) {
            case AUXTS_TOKEN_TYPE_ID:
                _cmd = command_handle_id(&token);
                break;
            case AUXTS_TOKEN_TYPE_STR:
                command_handle_str(_cmd, &token);
                break;
            case AUXTS_TOKEN_TYPE_BIN:
                command_handle_bin(_cmd, &token);
                break;
            case AUXTS_TOKEN_TYPE_PIPE:
                command_execution_plan_add_command(&plan, _cmd);
                break;
            case AUXTS_TOKEN_TYPE_INT:
                command_handle_int32(_cmd, &token);
                break;
            case AUXTS_TOKEN_TYPE_FLOAT:
                command_handle_float32(_cmd, &token);
                break;
            case AUXTS_TOKEN_TYPE_ERROR:
                return handle_error(token.as.err.msg, &in_buf, &out_buf);
            default:
                break;
        }

        token = auxts_parser_next_token(&parser);
    }

    command_execution_plan_add_command(&plan, _cmd);
    command_execution_plan_execute(&plan, exec, ctx, &in_buf, &out_buf);
    command_execution_plan_destroy(&plan);

    auxts_result_init(&result, out_buf.size);
    memcpy(result.data, out_buf.data, out_buf.size);

    msgpack_sbuffer_destroy(&in_buf);
    msgpack_sbuffer_destroy(&out_buf);

    return result;
}

void command_execution_plan_execute(auxts_command_execution_plan* plan, auxts_command_executor* exec, auxts_context* ctx, msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf) {
    msgpack_packer pk;

    for (int i = 0; i < plan->num_cmd; ++i) {
        msgpack_packer_init(&pk, in_buf, msgpack_sbuffer_write);

        auxts_command* cmd = plan->cmd[i];
        auxts_command_func func = command_executor_get(exec, cmd->name);
        if (!func) {
            handle_unknown_command(cmd, out_buf);
            break;
        }

        command_serialize_args(cmd, &pk);
        auxts_status status = func(in_buf, out_buf, ctx);
        msgpack_sbuffer_clear(in_buf);

        if (status != AUXTS_STATUS_OK) break;
    }
}

auxts_command_func command_executor_get(auxts_command_executor* exec, const char* cmd_name) {
    return auxts_kvstore_get(exec->kv, (void*)cmd_name);
}

void command_serialize_args(auxts_command* cmd, msgpack_packer* pk) {
    msgpack_pack_array(pk, cmd->num_args);

    for (int i = 0; i < cmd->num_args; ++i) {
        auxts_command_arg* arg = cmd->args[i];

        switch (arg->type) {
            case AUXTS_COMMAND_ARG_TYPE_STR:
                command_arg_serialize_str(arg, pk);
                break;
            case AUXTS_COMMAND_ARG_TYPE_BIN:
                command_arg_serialize_bin(arg, pk);
                break;
            case AUXTS_COMMAND_ARG_TYPE_INT:
                command_arg_serialize_int32(arg, pk);
                break;
            case AUXTS_COMMAND_ARG_TYPE_FLOAT:
                command_arg_serialize_float32(arg, pk);
                break;
        }
    }
}

void command_arg_serialize_str(auxts_command_arg* arg, msgpack_packer* pk) {
    msgpack_pack_str_with_body(pk, arg->as.str.ptr, arg->as.str.size);
}

void command_arg_serialize_bin(auxts_command_arg* arg, msgpack_packer* pk) {
    msgpack_pack_bin_with_body(pk, arg->as.bin.ptr, arg->as.bin.size);
}

void command_arg_serialize_int32(auxts_command_arg* arg, msgpack_packer* pk) {
    msgpack_pack_int32(pk, arg->as.i32);
}

void command_arg_serialize_float32(auxts_command_arg* arg, msgpack_packer* pk) {
    msgpack_pack_float(pk, arg->as.f32);
}

auxts_command* command_handle_id(auxts_token* token) {
    return command_create(token->as.id.ptr, token->as.id.size + 1);
}

void command_handle_str(auxts_command* cmd, auxts_token* token) {
    auxts_command_arg* arg = command_arg_create_str(token->as.str.ptr, token->as.str.size);
    command_add_arg(cmd, arg);
}

void command_handle_bin(auxts_command* cmd, auxts_token* token) {
    auxts_command_arg* arg = command_arg_create_bin(token->as.bin.ptr, token->as.bin.size);
    command_add_arg(cmd, arg);
}

void command_handle_int32(auxts_command* cmd, auxts_token* token) {
    auxts_command_arg* arg = command_arg_create_int32(token->as.i32);
    command_add_arg(cmd, arg);
}

void command_handle_float32(auxts_command* cmd, auxts_token* token) {
    auxts_command_arg* arg = command_arg_create_float32(token->as.f32);
    command_add_arg(cmd, arg);
}

void auxts_command_executor_init(auxts_command_executor* exec) {
    exec->kv = auxts_kvstore_create(10, command_executor_hash, command_executor_cmp, command_executor_destroy);
    auxts_kvstore_put(exec->kv, "EXTRACT", auxts_command_extract);
}

void auxts_command_executor_destroy(auxts_command_executor* exec) {
    auxts_kvstore_destroy(exec->kv);
}

auxts_result handle_error(const char* message, msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);
    auxts_serialize_status_error(&pk, message);

    auxts_result result;
    auxts_result_init(&result, out_buf->size);
    memcpy(result.data, out_buf->data, out_buf->size);

    msgpack_sbuffer_destroy(in_buf);
    msgpack_sbuffer_destroy(out_buf);

    return result;
}

void handle_unknown_command(auxts_command* cmd, msgpack_sbuffer* out_buf) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    char* message = malloc(strlen(cmd->name) + 20);
    sprintf(message, "Unknown command: %s", cmd->name);
    auxts_serialize_status_error(&pk, message);

    free(message);
}

auxts_command* command_create(const char* name, size_t size) {
    auxts_command* cmd = malloc(sizeof(auxts_command));
    if (!cmd) {
        perror("Failed to allocate auxts_command");
        return NULL;
    }

    cmd->num_args = 0;
    cmd->max_num_args = 4;

    strlcpy(cmd->name, name, size);
    to_uppercase(cmd->name);

    cmd->args = malloc(cmd->max_num_args * sizeof(auxts_command_arg*));
    if (!cmd->args) {
        perror("Failed to allocate args to auxts_command");
        free(cmd);
        return NULL;
    }

    return cmd;
}

void command_destroy(auxts_command* cmd) {
    for (int i = 0; i < cmd->num_args; ++i) {
        command_arg_destroy(cmd->args[i]);
    }

    free(cmd->args);
    free(cmd);
}

auxts_command_arg* command_arg_create() {
    auxts_command_arg* arg = malloc(sizeof(auxts_command_arg));
    if (!arg) {
        perror("Error allocating auxts_command_arg");
        return NULL;
    }

    return arg;
}

auxts_command_arg* command_arg_create_str(const char* ptr, size_t size) {
    auxts_command_arg* arg = command_arg_create();
    if (!arg) return NULL;

    arg->type = AUXTS_COMMAND_ARG_TYPE_STR;
    arg->as.str.ptr = ptr;
    arg->as.str.size = size;

    return arg;
}

auxts_command_arg* command_arg_create_bin(const char* ptr, size_t size) {
    auxts_command_arg* arg = command_arg_create();
    if (!arg) return NULL;

    arg->type = AUXTS_COMMAND_ARG_TYPE_BIN;
    arg->as.bin.ptr = ptr;
    arg->as.bin.size = size;

    return arg;
}

auxts_command_arg* command_arg_create_int32(int32_t d) {
    auxts_command_arg* arg = command_arg_create();
    if (!arg) return NULL;

    arg->type = AUXTS_COMMAND_ARG_TYPE_INT;
    arg->as.i32 = d;

    return arg;
}

auxts_command_arg* command_arg_create_float32(float d) {
    auxts_command_arg* arg = command_arg_create();
    if (!arg) return NULL;

    arg->type = AUXTS_COMMAND_ARG_TYPE_FLOAT;
    arg->as.f32 = d;

    return arg;
}

void command_arg_destroy(auxts_command_arg* arg) {
    free(arg);
}

void command_add_arg(auxts_command* cmd, auxts_command_arg* arg) {
    if (!cmd || !arg) return;

    if (cmd->num_args >= cmd->max_num_args) {
        cmd->max_num_args *= 2;

        auxts_command_arg** args = realloc(cmd->args, cmd->max_num_args * sizeof(auxts_command_arg*));
        if (!args) {
            perror("Failed to reallocate args to auxts_command");
            return;
        }

        cmd->args = args;
    }

    cmd->args[cmd->num_args] = arg;
    ++cmd->num_args;
}

void command_execution_plan_init(auxts_command_execution_plan* plan) {
    plan->num_cmd = 0;
    plan->max_num_cmd = 4;

    plan->cmd = malloc(plan->max_num_cmd * sizeof(auxts_command*));
    if (!plan->cmd) {
        perror("Failed to allocate commands to auxts_command_execution_plan");
        return;
    }
}

void command_execution_plan_destroy(auxts_command_execution_plan* plan) {
    for (int i = 0; i < plan->num_cmd; ++i) {
        command_destroy(plan->cmd[i]);
    }
    free(plan->cmd);
}

void command_execution_plan_add_command(auxts_command_execution_plan* plan, auxts_command* cmd) {
    if (!plan || !cmd) return;

    if (plan->num_cmd >= plan->max_num_cmd) {
        plan->max_num_cmd *= 2;

        auxts_command** _cmd = realloc(plan->cmd, plan->max_num_cmd * sizeof(auxts_command*));
        if (!_cmd) {
            perror("Error reallocating commands to auxts_command_execution_plan");
            return;
        }

        plan->cmd = _cmd;
    }

    plan->cmd[plan->num_cmd] = cmd;
    ++plan->num_cmd;
}

uint64_t command_executor_hash(void* key) {
    uint64_t hash[2];
    murmur3_x64_128(key, strlen((char*)key), 0, hash);
    return hash[0];
}

int command_executor_cmp(void* a, void* b) {
    uint64_t* x = (uint64_t*)a;
    uint64_t* y = (uint64_t*)b;
    return x[0] == y[0];
}

void command_executor_destroy(void* key, void* value) {}

void to_uppercase(char *str) {
    while (*str) {
        *str = (char)toupper(*str);
        str++;
    }
}

