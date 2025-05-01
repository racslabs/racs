
#ifndef RATS_COMMAND_H
#define RATS_COMMAND_H

#include <msgpack.h>
#include "scm.h"
#include "serialization.h"
#include "stream.h"
#include "extract.h"
#include "wav.h"
#include "format.h"

typedef enum {
    RATS_STATUS_OK,
    RATS_STATUS_NOT_FOUND,
    RATS_STATUS_ERROR
} rats_status;

#define rats_validate(pk, condition, error) \
    if (!(condition)) {                      \
        msgpack_sbuffer_clear(out_buf);      \
        return rats_serialize_error(pk, error); \
    }

#define rats_validate_type(pk, msg, arg_num, obj_type, error) \
    rats_validate(pk, obj_type == ((msg).data.via.array.ptr[arg_num].type), error)

#define rats_validate_num_args(pk, msg, num_args) \
    if ((msg).data.type == MSGPACK_OBJECT_ARRAY && (msg).data.via.array.size != (num_args)) \
        return rats_serialize_invalid_num_args(pk, num_args, (msg).data.via.array.size);

#define rats_create_command(name) \
    int rats_command_##name(msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf, rats_context* ctx)

rats_create_command(extract);

rats_create_command(eval);

rats_create_command(streamcreate);

rats_create_command(streaminfo);

rats_create_command(streamopen);

rats_create_command(streamclose);

rats_create_command(ping);

rats_create_command(format);

rats_create_command(streamlist);

rats_create_command(shutdown);

int rats_stream(msgpack_sbuffer *out_buf, rats_context *ctx, rats_uint8 *data);

#endif //RATS_COMMAND_H
