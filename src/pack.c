// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "pack.h"


int racs_pack_null_with_status_ok(msgpack_packer *pk) {
    msgpack_pack_nil(pk);
    return RACS_STATUS_OK;
}

int racs_pack_error(msgpack_packer *pk, const char * command, const char *message) {
    char *buf = NULL;
    asprintf(&buf, "%s: %s", command, message);

    msgpack_pack_ext_with_body(pk, buf, strlen(buf), RACS_EXT_TYPE_ERROR);
    free(buf);

    return RACS_STATUS_ERROR;
}

int racs_pack_str(msgpack_packer *pk, const char *str) {
    msgpack_pack_str_with_body(pk, str, strlen(str));
    return RACS_STATUS_OK;
}

int racs_pack_int64(msgpack_packer *pk, racs_int64 d) {
    msgpack_pack_int64(pk, d);
    return RACS_STATUS_OK;
}

int racs_pack_uint64(msgpack_packer *pk, racs_uint64 d) {
    msgpack_pack_uint64(pk, d);
    return RACS_STATUS_OK;
}

int racs_pack_float64(msgpack_packer *pk, double d) {
    msgpack_pack_double(pk, d);
    return RACS_STATUS_OK;
}

int racs_pack_bool(msgpack_packer *pk, bool d) {
    d ? msgpack_pack_true(pk) : msgpack_pack_false(pk);
    return RACS_STATUS_OK;
}

int racs_pack_s8v(msgpack_packer *pk, racs_int8 *data, size_t n) {
    msgpack_pack_ext_with_body(pk, data, n * sizeof(racs_int8), RACS_EXT_TYPE_S8VEC);
    return RACS_STATUS_OK;
}

int racs_pack_u8v(msgpack_packer *pk, racs_uint8 *data, size_t n) {
    msgpack_pack_ext_with_body(pk, data, n * sizeof(racs_uint8), RACS_EXT_TYPE_U8VEC);
    return RACS_STATUS_OK;
}

int racs_pack_s16v(msgpack_packer *pk, racs_int16 *data, size_t n) {
    msgpack_pack_ext_with_body(pk, data, n * sizeof(racs_int16), RACS_EXT_TYPE_S16VEC);
    return RACS_STATUS_OK;
}

int racs_pack_u16v(msgpack_packer *pk, racs_uint16 *data, size_t n) {
    msgpack_pack_ext_with_body(pk, data, n * sizeof(racs_uint16), RACS_EXT_TYPE_U16VEC);
    return RACS_STATUS_OK;
}

int racs_pack_s32v(msgpack_packer *pk, racs_int32 *data, size_t n) {
    msgpack_pack_ext_with_body(pk, data, n * sizeof(racs_int32), RACS_EXT_TYPE_S32VEC);
    return RACS_STATUS_OK;
}

int racs_pack_s32v_without_metadata(msgpack_packer *pk, racs_int32 *data, size_t n) {
    msgpack_pack_ext_with_body(pk, data + 8, n * sizeof(racs_int32) - 8, RACS_EXT_TYPE_S32VEC);
    return RACS_STATUS_OK;
}

int racs_pack_u32v(msgpack_packer *pk, racs_uint32 *data, size_t n) {
    msgpack_pack_ext_with_body(pk, data, n * sizeof(racs_uint32), RACS_EXT_TYPE_U32VEC);
    return RACS_STATUS_OK;
}

int racs_pack_f32v(msgpack_packer *pk, float *data, size_t n) {
    msgpack_pack_ext_with_body(pk, data, n * sizeof(float), RACS_EXT_TYPE_F32VEC);
    return RACS_STATUS_OK;
}

int racs_pack_c64v(msgpack_packer *pk, racs_complex *data, size_t n) {
    msgpack_pack_ext_with_body(pk, data, n * sizeof(racs_complex), RACS_EXT_TYPE_C64VEC);
    return RACS_STATUS_OK;
}

int racs_pack_invalid_num_args(msgpack_packer *pk, const char *command, int expected, int actual) {
    char message[255];
    sprintf(message, "Expected %d args, but got %d", expected, actual);
    return racs_pack_error(pk, command, message);
}

int racs_pack_streams(msgpack_packer *pk, racs_streams *streams) {
    msgpack_pack_array(pk, streams->num_streams);

    for (int i = 0; i < streams->num_streams; ++i) {
        msgpack_pack_str_with_body(pk, streams->streams[i], strlen(streams->streams[i]));
    }

    return RACS_STATUS_OK;
}

char *racs_unpack_str(msgpack_object *obj) {
    size_t size = obj->via.str.size + 1;

    char *str = malloc(size);
    snprintf(str, size, "%s", obj->via.str.ptr);

    return str;
}

char *racs_unpack_str_from_array(msgpack_object *obj, int n) {
    size_t size = obj->via.array.ptr[n].via.str.size + 1;

    char *str = malloc(size);
    snprintf(str, size, "%s", obj->via.array.ptr[n].via.str.ptr);

    return str;
}

racs_uint8 *racs_unpack_u8v(msgpack_object *obj) {
    if (obj->type == MSGPACK_OBJECT_EXT && obj->via.ext.type == RACS_EXT_TYPE_U8VEC)
        return (racs_uint8 *) obj->via.ext.ptr;

    return NULL;
}

racs_int16 *racs_unpack_s16v(msgpack_object *obj) {
    if (obj->type == MSGPACK_OBJECT_EXT && obj->via.ext.type == RACS_EXT_TYPE_S16VEC)
        return (racs_int16 *) obj->via.ext.ptr;

    return NULL;
}

racs_int32 *racs_unpack_s32v(msgpack_object *obj) {
    if (obj->type == MSGPACK_OBJECT_EXT && obj->via.ext.type == RACS_EXT_TYPE_S32VEC)
        return (racs_int32 *) obj->via.ext.ptr;

    return NULL;
}

size_t racs_unpack_u8v_size(msgpack_object *obj) {
    return obj->via.ext.size;
}

size_t racs_unpack_s16v_size(msgpack_object *obj) {
    return obj->via.ext.size / sizeof(racs_int16);
}

size_t racs_unpack_s32v_size(msgpack_object *obj) {
    return obj->via.ext.size / sizeof(racs_int32);
}

racs_uint16 racs_unpack_uint16(msgpack_object *obj) {
    return (racs_uint16) obj->via.u64;
}

racs_uint16 racs_unpack_uint16_from_array(msgpack_object *obj, int n) {
    return (racs_uint16) obj->via.array.ptr[n].via.u64;
}

racs_int32 racs_unpack_int32(msgpack_object *obj) {
    return (racs_int32) obj->via.i64;
}

racs_int32 racs_unpack_int32_from_array(msgpack_object *obj, int n) {
    return (racs_int32) obj->via.array.ptr[n].via.i64;
}

racs_uint32 racs_unpack_uint32(msgpack_object *obj) {
    return (racs_uint32) obj->via.u64;
}

racs_uint32 racs_unpack_uint32_from_array(msgpack_object *obj, int n) {
    return (racs_uint32) obj->via.array.ptr[n].via.u64;
}

racs_int64 racs_unpack_int64(msgpack_object *obj) {
    return obj->via.i64;
}

racs_int64 racs_unpack_int64_from_array(msgpack_object *obj, int n) {
    return obj->via.array.ptr[n].via.i64;
}

racs_uint64 racs_unpack_uint64(msgpack_object *obj) {
    return obj->via.u64;
}

racs_uint64 racs_unpack_uint64_from_array(msgpack_object *obj, int n) {
    return obj->via.array.ptr[n].via.u64;
}

float racs_unpack_float32(msgpack_object *obj) {
    return (float)obj->via.f64;
}

float racs_unpack_float32_from_array(msgpack_object *obj, int n) {
    return (float)obj->via.array.ptr[n].via.f64;
}

double racs_unpack_float64(msgpack_object *obj) {
    return obj->via.f64;
}

double racs_unpack_float64_from_array(msgpack_object *obj, int n) {
    return obj->via.array.ptr[n].via.f64;
}
