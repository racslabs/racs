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
#include "extract.h"
#include "wav.h"
#include "format.h"

typedef enum {
    RACS_STATUS_OK,
    RACS_STATUS_NOT_FOUND,
    RACS_STATUS_ERROR
} racs_status;

#define racs_validate(pk, condition, error) \
    if (!(condition)) {                      \
        msgpack_sbuffer_clear(out_buf);      \
        return racs_pack_error(pk, error); \
    }

#define racs_validate_type(pk, msg, arg_num, obj_type, error) \
    racs_validate(pk, obj_type == ((msg).data.via.array.ptr[arg_num].type), error)

#define racs_validate_num_args(pk, msg, num_args) \
    if ((msg).data.type == MSGPACK_OBJECT_ARRAY && (msg).data.via.array.size != (num_args)) \
        return racs_pack_invalid_num_args(pk, num_args, (msg).data.via.array.size);

#define racs_create_command(name) \
    int racs_command_##name(msgpack_sbuffer* in_buf, msgpack_sbuffer* out_buf, racs_context* ctx)

racs_create_command(extract);

racs_create_command(eval);

racs_create_command(streamcreate);

racs_create_command(streaminfo);

racs_create_command(streamopen);

racs_create_command(streamclose);

racs_create_command(ping);

racs_create_command(format);

racs_create_command(streamlist);

racs_create_command(shutdown);

int racs_stream(msgpack_sbuffer *out_buf, racs_context *ctx, racs_uint8 *data);

int racs_stream_batch(msgpack_sbuffer *out_buf, racs_context *ctx, racs_uint8 *data, size_t size);

#ifdef __cplusplus
}
#endif

#endif //RACS_COMMAND_H
