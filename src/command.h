// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_COMMAND_H
#define RACS_COMMAND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <msgpack.h>
#include "scm.h"
#include "pack.h"
#include "stream.h"
#include "range.h"
#include "wav.h"
#include "encode.h"
#include "ops.h"

typedef enum {
    RACS_STATUS_OK,
    RACS_STATUS_NOT_FOUND,
    RACS_STATUS_ERROR
} racs_status;

#define racs_validate(pk, condition, command, error) \
    if (!(condition)) {                      \
        msgpack_sbuffer_clear(out_buf);      \
        return racs_pack_error(pk, command, error); \
    }

#define racs_validate_arg_type(pk, msg, arg_num, obj_type, command, error) \
    racs_validate(pk, obj_type == ((msg).data.via.array.ptr[arg_num].type), command, error)

#define racs_validate_num_args(pk, msg, command, num_args) \
    if ((msg).data.type == MSGPACK_OBJECT_ARRAY && (msg).data.via.array.size != (num_args)) \
        return racs_pack_invalid_num_args(pk, command, num_args, (msg).data.via.array.size);

#define racs_validate_not_null(pk, msg, command) \
    if ((msg).data.type == MSGPACK_OBJECT_NIL) { \
        msgpack_sbuffer_clear(out_buf); \
        return racs_pack_error(pk, command, "Missing input data."); \
    }

#define racs_validate_s32v(pk, msg, command) \
    char *type = racs_unpack_str(msg.data, 0); \
    if (strcmp(type, "s32v") != 0) { \
        free(type); \
        msgpack_sbuffer_clear(out_buf); \
        return racs_pack_error(pk, command, "Invalid input type. Expected: int32 array"); \
    } \
    free(type);

#define racs_create_command(name) \
    int racs_command_##name(msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf, racs_context* ctx, bool is_final)

racs_create_command(range);

racs_create_command(eval);

racs_create_command(streamcreate);

racs_create_command(metadata);

racs_create_command(streamopen);

racs_create_command(streamclose);

racs_create_command(ping);

racs_create_command(encode);

racs_create_command(streamlist);

racs_create_command(shutdown);

racs_create_command(gain);

racs_create_command(trim);

racs_create_command(fade);

racs_create_command(pan);

racs_create_command(pad);

racs_create_command(clip);

racs_create_command(split);

int racs_stream(msgpack_sbuffer *out_buf, racs_context *ctx, racs_uint8 *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif //RACS_COMMAND_H
