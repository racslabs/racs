
#ifndef AUXTS_COMMANDS_H
#define AUXTS_COMMANDS_H

#include <msgpack.h>
#include "serialization.h"

typedef enum {
    AUXTS_COMMAND_STATUS_OK,
    AUXTS_COMMAND_STATUS_NO_DATA,
    AUXTS_COMMAND_STATUS_ERROR
} auxts_command_status;

#define auxts_create_command(name) \
    int auxts_command_##name(msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf, auxts_context* ctx)

auxts_create_command(extract);

#endif //AUXTS_COMMANDS_H
