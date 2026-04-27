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

#ifdef __cplusplus
extern "C" {
#endif

#include <msgpack.h>
#include "range.h"
#include "exec.h"
#include "types.h"
#include "stream.h"

typedef enum {
    RACS_EXT_TYPE_ERROR,
    RACS_EXT_TYPE_U8VEC,
    RACS_EXT_TYPE_S8VEC,
    RACS_EXT_TYPE_U16VEC,
    RACS_EXT_TYPE_S16VEC,
    RACS_EXT_TYPE_U32VEC,
    RACS_EXT_TYPE_S32VEC,
    RACS_EXT_TYPE_F32VEC,
    RACS_EXT_TYPE_C64VEC
} racs_ext_type;

#define racs_parse_buf(buf, pk, msg, command, message) \
    if (msgpack_unpack_next(msg, (buf)->data, (buf)->size, 0) == MSGPACK_UNPACK_PARSE_ERROR) { \
        return racs_pack_error((pk), command , message);\
    }

int racs_pack_null_with_status_ok(msgpack_packer *pk);

int racs_pack_error(msgpack_packer *pk, const char * command, const char *message);

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

int racs_pack_s32v_without_metadata(msgpack_packer *pk, racs_int32 *data, size_t n);

int racs_pack_invalid_num_args(msgpack_packer *pk, const char *command, int expected, int actual);

int racs_pack_streams(msgpack_packer *pk, racs_streams *streams);

char *racs_unpack_str(msgpack_object *obj);

char *racs_unpack_str_from_array(msgpack_object *obj, int n);

racs_uint16 racs_unpack_uint16(msgpack_object *obj);

racs_uint16 racs_unpack_uint16_from_array(msgpack_object *obj, int n);

racs_int32 racs_unpack_int32(msgpack_object *obj);

racs_int32 racs_unpack_int32_from_array(msgpack_object *obj, int n);

racs_uint32 racs_unpack_uint32(msgpack_object *obj);

racs_uint32 racs_unpack_uint32_from_array(msgpack_object *obj, int n);

racs_int64 racs_unpack_int64(msgpack_object *obj);

racs_int64 racs_unpack_int64_from_array(msgpack_object *obj, int n);

racs_uint64 racs_unpack_uint64(msgpack_object *obj);

racs_uint64 racs_unpack_uint64_from_array(msgpack_object *obj, int n);

racs_uint8 *racs_unpack_u8v(msgpack_object *obj);

racs_int16 *racs_unpack_s16v(msgpack_object *obj);

racs_int32 *racs_unpack_s32v(msgpack_object *obj);

size_t racs_unpack_u8v_size(msgpack_object *obj);

size_t racs_unpack_s16v_size(msgpack_object *obj);

size_t racs_unpack_s32v_size(msgpack_object *obj);

float racs_unpack_float32(msgpack_object *obj);

float racs_unpack_float32_from_array(msgpack_object *obj, int n);

double racs_unpack_float64(msgpack_object *obj);

double racs_unpack_float64_from_array(msgpack_object *obj, int n);

#ifdef __cplusplus
}
#endif

#endif //RACS_PACK_H
