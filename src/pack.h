// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#ifndef RACS_PACK_H
#define RACS_PACK_H

#include <msgpack.h>
#include "extract.h"
#include "exec.h"
#include "types.h"
#include "stream.h"

typedef enum {
    RACS_TYPE_STR,
    RACS_TYPE_INT,
    RACS_TYPE_FLOAT,
    RACS_TYPE_LIST,
    RACS_TYPE_NULL,
    RACS_TYPE_ERROR,
    RACS_TYPE_BOOL,
    RACS_TYPE_U8VEC,
    RACS_TYPE_S8VEC,
    RACS_TYPE_U16VEC,
    RACS_TYPE_S16VEC,
    RACS_TYPE_U32VEC,
    RACS_TYPE_S32VEC,
    RACS_TYPE_F32VEC,
    RACS_TYPE_C64VEC
} racs_type;

#define racs_parse_buf(buf, pk, msg, message) \
    if (msgpack_unpack_next(msg, (buf)->data, (buf)->size, 0) == MSGPACK_UNPACK_PARSE_ERROR) { \
        return racs_pack_error((pk), message);\
    }

extern const char *const racs_type_string[];

int racs_pack_null_with_status_ok(msgpack_packer *pk);

int racs_pack_null_with_status_not_found(msgpack_packer *pk);

int racs_pack_error(msgpack_packer *pk, const char *message);

int racs_pack_str(msgpack_packer *pk, const char *str);

int racs_pack_int64(msgpack_packer *pk, racs_int64 d);

int racs_pack_uint64(msgpack_packer *pk, racs_uint64 d);

int racs_pack_float64(msgpack_packer *pk, double d);

int racs_pack_bool(msgpack_packer *pk, bool d);

int racs_pack_s8v(msgpack_packer *pk, racs_int8 *data, size_t n);

int racs_pack_u8v(msgpack_packer *pk, racs_uint8 *data, size_t n);

int racs_pack_s16v(msgpack_packer *pk, racs_int16 *data, size_t n);

int racs_pack_u16v(msgpack_packer *pk, racs_uint16 *data, size_t n);

int racs_pack_s32v(msgpack_packer *pk, racs_int32 *data, size_t n);

int racs_pack_u32v(msgpack_packer *pk, racs_uint32 *data, size_t n);

int racs_pack_f32v(msgpack_packer *pk, float *data, size_t n);

int racs_pack_c64v(msgpack_packer *pk, racs_complex *data, size_t n);

void racs_pack_type(msgpack_packer *pk, int type);

int racs_pack_invalid_num_args(msgpack_packer *pk, int expected, int actual);

int racs_pack_streams(msgpack_packer *pk, racs_streams *streams);

char *racs_unpack_str(msgpack_object *obj, int n);

racs_uint16 racs_unpack_uint16(msgpack_object *obj, int n);

racs_int32 racs_unpack_int32(msgpack_object *obj, int n);

racs_uint32 racs_unpack_uint32(msgpack_object *obj, int n);

racs_int64 racs_unpack_int64(msgpack_object *obj, int n);

racs_uint64 racs_unpack_uint64(msgpack_object *obj, int n);

racs_uint8 *racs_unpack_u8v(msgpack_object *obj, int n);

racs_int16 *racs_unpack_s16v(msgpack_object *obj, int n);

racs_int32 *racs_unpack_s32v(msgpack_object *obj, int n);

size_t racs_unpack_u8v_size(msgpack_object *obj, int n);

size_t racs_unpack_s16v_size(msgpack_object *obj, int n);

size_t racs_unpack_s32v_size(msgpack_object *obj, int n);

float racs_unpack_float32(msgpack_object *obj, int n);

int racs_is_object_type(msgpack_object *obj, msgpack_object_type type, int arg_num);

#endif //RACS_PACK_H
