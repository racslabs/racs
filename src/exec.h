
#ifndef RATS_EXEC_H
#define RATS_EXEC_H

#include "kvstore.h"
#include "parser.h"
#include "command.h"

typedef enum {
    RATS_COMMAND_ARG_TYPE_STR,
    RATS_COMMAND_ARG_TYPE_INT,
    RATS_COMMAND_ARG_TYPE_FLOAT
} rats_command_arg_type;

typedef enum {
    RATS_COMMAND_OP_PIPE,
    RATS_COMMAND_OP_NONE
} rats_command_op;

typedef enum {
    RATS_EXEC_STATUS_CONTINUE,
    RATS_EXEC_STATUS_ABORT
} rats_exec_status;

typedef struct {
    rats_kvstore *kv;
} rats_exec;

typedef struct {
    const char *ptr;
    size_t size;
} rats_command_arg_;

typedef rats_command_arg_ rats_command_arg_str;

typedef union {
    double f64;
    rats_int64 i64;
    rats_uint64 u64;
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
    rats_command_arg **args;
} rats_command;

typedef struct {
    int num_cmd;
    int max_num_cmd;
    rats_command **cmd;
} rats_exec_plan;

typedef int (*rats_command_func)(msgpack_sbuffer *in_buf, msgpack_sbuffer *out_buf, rats_context *ctx);

void rats_exec_init(rats_exec *exec);

void rats_exec_destroy(rats_exec *exec);

rats_result rats_exec_exec(rats_exec *exec, rats_context *ctx, const char *cmd);

rats_result rats_exec_stream(rats_context *ctx, rats_uint8 *data);

rats_uint64 exec_hash(void *key);

int exec_cmp(void *a, void *b);

void exec_destroy(void *key, void *value);

rats_command_func exec_get(rats_exec *exec, const char *cmd_name);

rats_command *command_create(const char *name, rats_command_op op, size_t size);

void command_destroy(rats_command *cmd);

rats_command_arg *command_arg_create();

rats_command_arg *command_arg_create_str(const char *ptr, size_t size);

rats_command_arg *command_arg_create_int64(rats_int64 d);

rats_command_arg *command_arg_create_float64(double d);

void command_arg_destroy(rats_command_arg *arg);

rats_command *handle_id(rats_token *curr, rats_token *prev);

void handle_error(const char *message, msgpack_sbuffer *out_buf);

void handle_unknown_command(rats_command *cmd, msgpack_sbuffer *out_buf);

int command_handle_id(rats_command *cmd, msgpack_sbuffer *out_buf);

int command_handle_str(rats_command *cmd, msgpack_sbuffer *out_buf, rats_token *curr, rats_token *prev);

int command_handle_int64(rats_command *cmd, msgpack_sbuffer *out_buf, rats_token *curr, rats_token *prev);

int command_handle_float64(rats_command *cmd, msgpack_sbuffer *out_buf, rats_token *curr, rats_token *prev);

int command_handle_time(rats_command *cmd, msgpack_sbuffer *out_buf, rats_token *curr, rats_token *prev);

void command_add_arg(rats_command *cmd, rats_command_arg *arg);

void command_serialize_args(rats_command *cmd, msgpack_packer *pk);

void command_arg_serialize_str(rats_command_arg *arg, msgpack_packer *pk);

void command_arg_serialize_int64(rats_command_arg *arg, msgpack_packer *pk);

void command_arg_serialize_float64(rats_command_arg *arg, msgpack_packer *pk);

void exec_plan_init(rats_exec_plan *plan);

void exec_plan_destroy(rats_exec_plan *plan);

void exec_plan_add_command(rats_exec_plan *plan, rats_command *cmd);

void exec_plan_exec(rats_exec_plan *plan, rats_exec *exec, rats_context *ctx, msgpack_sbuffer *in_buf,
                    msgpack_sbuffer *out_buf);

int exec_plan_build(rats_exec_plan *plan, msgpack_sbuffer *out_buf, rats_parser *parser);

void to_uppercase(char *str);

#endif //RATS_EXEC_H
