
#ifndef AUXTS_EXEC_H
#define AUXTS_EXEC_H

#include "kvstore.h"
#include "parser.h"
#include "command.h"

typedef enum {
    AUXTS_COMMAND_ARG_TYPE_STR,
    AUXTS_COMMAND_ARG_TYPE_INT,
    AUXTS_COMMAND_ARG_TYPE_FLOAT
} rats_command_arg_type;

typedef enum {
    AUXTS_COMMAND_OP_PIPE,
    AUXTS_COMMAND_OP_NONE
} rats_command_op;

typedef enum {
    AUXTS_EXEC_STATUS_CONTINUE,
    AUXTS_EXEC_STATUS_ABORT
} rats_exec_status;

typedef struct {
    rats_kvstore* kv;
} rats_exec;

typedef struct {
    const char* ptr;
    size_t size;
} rats_command_arg_;

typedef rats_command_arg_ rats_command_arg_str;

typedef union {
    double                f64;
    int64_t               i64;
    uint64_t              u64;
    rats_command_arg_str str;
} rats_command_arg_union;

typedef struct {
    rats_command_arg_type type;
    rats_command_arg_union as;
} rats_command_arg;

typedef struct {
    char name[255];
    int num_args;
    int max_num_args;
    rats_command_op op;
    rats_command_arg** args;
} rats_command;

typedef struct {
    int num_cmd;
    int max_num_cmd;
    rats_command** cmd;
} rats_exec_plan;

typedef int (*rats_command_func)(msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf, rats_context* ctx);

void rats_exec_init(rats_exec* exec);
void rats_exec_destroy(rats_exec* exec);
rats_result rats_exec_exec(rats_exec* exec, rats_context* ctx, const char* cmd);
rats_result rats_exec_stream(rats_context* ctx, rats_uint8* data);

#endif //AUXTS_EXEC_H
