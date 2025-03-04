#include "command.h"

static uint64_t command_executor_hash(void* key);
static int command_executor_cmp(void* a, void* b);
static void command_executor_destroy(void* key, void* value);
static auxts_command* command_create(const char* name);
static void command_destroy(auxts_command* cmd);
static auxts_command_arg* command_arg_create();
static auxts_command_arg* command_arg_create_str(const char* ptr, size_t size);
static auxts_command_arg* command_arg_create_bin(const char* ptr, size_t size);
static auxts_command_arg* command_arg_create_int32(int32_t d);
static auxts_command_arg* command_arg_create_float32(float d);
static void command_arg_destroy(auxts_command_arg* arg);
static void command_add_arg(auxts_command* cmd, auxts_command_arg* arg);
static void command_execution_plan_init(auxts_command_execution_plan* plan);
static void command_execution_plan_destroy(auxts_command_execution_plan* plan);
static void command_execution_plan_add_command(auxts_command_execution_plan* plan, auxts_command* cmd);

auxts_result auxts_command_executor_execute(auxts_command_executor* exec, auxts_context* ctx, const char* cmd) {
    auxts_result result;

    auxts_parser parser;
    auxts_parser_init(&parser, cmd);

    auxts_command_execution_plan plan;
    command_execution_plan_init(&plan);

    auxts_command* _cmd = NULL;

    auxts_token token = auxts_parser_next_token(&parser);
    if (token.type != AUXTS_TOKEN_TYPE_ID) {
        //TODO: add error message
        return result;
    }

    while (token.type != AUXTS_TOKEN_TYPE_EOF) {
        switch (token.type) {
            case AUXTS_TOKEN_TYPE_ID: {
                _cmd = command_create(token.as.id.ptr);
            }
                break;
            case AUXTS_TOKEN_TYPE_STR: {
                auxts_command_arg* arg = command_arg_create_str(token.as.str.ptr, token.as.str.size);
                command_add_arg(_cmd, arg);
            }
                break;
            case AUXTS_TOKEN_TYPE_BIN: {
                auxts_command_arg* arg = command_arg_create_bin(token.as.bin.ptr, token.as.bin.size);
                command_add_arg(_cmd, arg);
            }
                break;
            case AUXTS_TOKEN_TYPE_PIPE: {
                command_execution_plan_add_command(&plan, _cmd);
            }
                break;
            case AUXTS_TOKEN_TYPE_INT: {
                auxts_command_arg* arg = command_arg_create_int32(token.as.i32);
                command_add_arg(_cmd, arg);
            }
                break;
            case AUXTS_TOKEN_TYPE_FLOAT: {
                auxts_command_arg* arg = command_arg_create_float32(token.as.f32);
                command_add_arg(_cmd, arg);
            }
                break;
            case AUXTS_TOKEN_TYPE_EOF:
                break;
            case AUXTS_TOKEN_TYPE_ERROR:
                return result;
        }

        token = auxts_parser_next_token(&parser);
    }

    command_execution_plan_add_command(&plan, _cmd);

    return result;
}

void auxts_command_executor_init(auxts_command_executor* exec) {
    exec->kv = auxts_kvstore_create(10, command_executor_hash, command_executor_cmp, command_executor_destroy);
    auxts_kvstore_put(exec->kv, "extract", auxts_extract);
}

void auxts_command_executor_destroy(auxts_command_executor* exec) {
    auxts_kvstore_destroy(exec->kv);
}

auxts_command* command_create(const char* name) {
    auxts_command* cmd = malloc(sizeof(auxts_command));
    if (!cmd) {
        perror("Failed to allocate auxts_command");
        return NULL;
    }

    cmd->name = name;
    cmd->num_args = 0;
    cmd->max_num_args = 4;

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
            perror("Filed to reallocate args to auxts_command");
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
    murmur3_x64_128(key, 2 * sizeof(uint64_t), 0, hash);
    return hash[0];
}

int command_executor_cmp(void* a, void* b) {
    uint64_t* x = (uint64_t*)a;
    uint64_t* y = (uint64_t*)b;
    return x[0] == y[0] && x[1] == y[1];
}

void command_executor_destroy(void* key, void* value) {}

