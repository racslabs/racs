
#ifndef AUXTS_COMMANDS_H
#define AUXTS_COMMANDS_H

#include <msgpack.h>
#include "serialization.h"

typedef enum {
    AUXTS_COMMAND_STATUS_OK,
    AUXTS_COMMAND_STATUS_NO_DATA,
    AUXTS_COMMAND_STATUS_ERROR
} auxts_command_status;

#define AUXTS_CHECK_NUM_ARGS(pk, obj, num_args) \
    if ((obj).type == MSGPACK_OBJECT_ARRAY && (obj).via.array.size != (num_args)) { \
        auxts_serialize_invalid_num_args(pk, num_args, (obj).via.array.size);       \
        return AUXTS_COMMAND_STATUS_ERROR;      \
    }

#define AUXTS_CREATE_COMMAND(name) \
    int auxts_command_##name(msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf, auxts_context* ctx)

AUXTS_CREATE_COMMAND(extract);

#endif //AUXTS_COMMANDS_H
