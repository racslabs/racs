
#ifndef AUXTS_COMMAND_EXECUTOR_H
#define AUXTS_COMMAND_EXECUTOR_H

#include "kvstore.h"
#include "parser.h"
#include "commands.h"

typedef enum {
    AUXTS_COMMAND_ARG_TYPE_STR,
    AUXTS_COMMAND_ARG_TYPE_BIN,
    AUXTS_COMMAND_ARG_TYPE_INT,
    AUXTS_COMMAND_ARG_TYPE_FLOAT
} auxts_command_arg_type;

typedef enum {
    AUXTS_COMMAND_OP_PIPE,
    AUXTS_COMMAND_OP_TILDE,
    AUXTS_COMMAND_OP_NONE
} auxts_command_op;

typedef struct {
    int merge_count;
    auxts_kvstore* kv;
} auxts_command_executor;

typedef struct {
    const char* ptr;
    size_t size;
} auxts_command_arg_;

typedef auxts_command_arg_ auxts_command_arg_str;
typedef auxts_command_arg_ auxts_command_arg_bin;

typedef union {
    float                 f32;
    int32_t               i32;
    uint32_t              u32;
    auxts_command_arg_str str;
    auxts_command_arg_bin bin;
} auxts_command_arg_union;

typedef struct {
    auxts_command_arg_type type;
    auxts_command_arg_union as;
} auxts_command_arg;

typedef struct {
    char name[255];
    int num_args;
    int max_num_args;
    auxts_command_op op;
    auxts_command_arg** args;
} auxts_command;

typedef struct {
    int num_cmd;
    int max_num_cmd;
    auxts_command** cmd;
} auxts_command_execution_plan;

typedef int (*auxts_command_func)(msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf, auxts_context* ctx);

void auxts_command_executor_init(auxts_command_executor* exec);
void auxts_command_executor_destroy(auxts_command_executor* exec);
auxts_result auxts_command_executor_execute(auxts_command_executor* exec, auxts_context* ctx, const char* cmd);

#endif //AUXTS_COMMAND_EXECUTOR_H
