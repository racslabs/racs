
#ifndef AUXTS_EXEC_H
#define AUXTS_EXEC_H

#include "kvstore.h"
#include "parser.h"
#include "command.h"

typedef enum {
    AUXTS_COMMAND_ARG_TYPE_STR,
    AUXTS_COMMAND_ARG_TYPE_INT,
    AUXTS_COMMAND_ARG_TYPE_FLOAT
} auxts_command_arg_type;

typedef enum {
    AUXTS_COMMAND_OP_PIPE,
    AUXTS_COMMAND_OP_NONE
} auxts_command_op;

typedef enum {
    AUXTS_EXEC_STATUS_CONTINUE,
    AUXTS_EXEC_STATUS_ABORT
} auxts_exec_status;

typedef struct {
    auxts_kvstore* kv;
} auxts_exec;

typedef struct {
    const char* ptr;
    size_t size;
} auxts_command_arg_;

typedef auxts_command_arg_ auxts_command_arg_str;

typedef union {
    double                f64;
    int64_t               i64;
    uint64_t              u64;
    auxts_command_arg_str str;
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
} auxts_exec_plan;

typedef int (*auxts_command_func)(msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf, auxts_context* ctx);

void auxts_exec_init(auxts_exec* exec);
void auxts_exec_destroy(auxts_exec* exec);
auxts_result auxts_exec_exec(auxts_exec* exec, auxts_context* ctx, const char* cmd);
auxts_result auxts_exec_stream(auxts_context* ctx, auxts_uint8* data);

#endif //AUXTS_EXEC_H
