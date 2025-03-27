
#ifndef AUXTS_COMMANDS_H
#define AUXTS_COMMANDS_H

#include <msgpack.h>
#include "scm.h"
#include "serialization.h"
#include "create.h"

typedef enum {
    AUXTS_STATUS_OK,
    AUXTS_STATUS_NOT_FOUND,
    AUXTS_STATUS_ERROR
} auxts_status;

#define auxts_validate(pk, condition, error) \
    if (!(condition)) return auxts_serialize_error(pk, error);

#define auxts_validate_arg_type(pk, msg, arg_num, obj_type, error) \
    auxts_validate(pk, obj_type == ((msg).data.via.array.ptr[arg_num].type), error)

#define auxts_validate_num_args(pk, msg, num_args) \
    if ((msg).data.type == MSGPACK_OBJECT_ARRAY && (msg).data.via.array.size != (num_args)) \
        return auxts_serialize_invalid_num_args(pk, num_args, (msg).data.via.array.size);

#define auxts_create_command(name) \
    int auxts_command_##name(msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf, auxts_context* ctx)

auxts_create_command(extract);
auxts_create_command(eval);
auxts_create_command(create);
auxts_create_command(metadata);
auxts_create_command(ping);

#endif //AUXTS_COMMANDS_H
