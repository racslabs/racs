
#ifndef AUXTS_COMMANDS_H
#define AUXTS_COMMANDS_H

#include <msgpack.h>
#include "serialization.h"

#define auxts_create_command(name) \
    void auxts_command_##name(msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf, auxts_context* ctx)

auxts_create_command(extract);

#endif //AUXTS_COMMANDS_H
