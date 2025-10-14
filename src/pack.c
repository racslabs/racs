// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "pack.h"


const char *const racs_type_string[] = {
        "string",
        "int",
        "float",
        "list",
        "null",
        "error",
        "bool",
        "u8v",
        "s8v",
        "u16v",
        "s16v",
        "u32v",
        "s32v",
        "f32v",
        "c64v"
};

void racs_pack_type(msgpack_packer *pk, int type) {
    const char *type_string = racs_type_string[type];
    msgpack_pack_str_with_body(pk, type_string, strlen(type_string));
}

int racs_pack_null_with_status_ok(msgpack_packer *pk) {
    msgpack_pack_array(pk, 1);
    racs_pack_type(pk, RACS_TYPE_NULL);
    return RACS_STATUS_OK;
}

int racs_pack_null_with_status_not_found(msgpack_packer *pk) {
    msgpack_pack_array(pk, 1);
    racs_pack_type(pk, RACS_TYPE_NULL);
    return RACS_STATUS_NOT_FOUND;
}

int racs_pack_error(msgpack_packer *pk, const char *message) {
    msgpack_pack_array(pk, 2);
    racs_pack_type(pk, RACS_TYPE_ERROR);
    msgpack_pack_str_with_body(pk, message, strlen(message));
    return RACS_STATUS_ERROR;
}

int racs_pack_str(msgpack_packer *pk, const char *str) {
    msgpack_pack_array(pk, 2);
    racs_pack_type(pk, RACS_TYPE_STR);
    msgpack_pack_str_with_body(pk, str, strlen(str));
    return RACS_STATUS_OK;
}

int racs_pack_int64(msgpack_packer *pk, racs_int64 d) {
    msgpack_pack_array(pk, 2);
    racs_pack_type(pk, RACS_TYPE_INT);
    msgpack_pack_int64(pk, d);
    return RACS_STATUS_OK;
}

int racs_pack_uint64(msgpack_packer *pk, racs_uint64 d) {
    msgpack_pack_array(pk, 2);
    racs_pack_type(pk, RACS_TYPE_INT);
    msgpack_pack_uint64(pk, d);
    return RACS_STATUS_OK;
}

int racs_pack_float64(msgpack_packer *pk, double d) {
    msgpack_pack_array(pk, 2);
    racs_pack_type(pk, RACS_TYPE_FLOAT);
    msgpack_pack_double(pk, d);
    return RACS_STATUS_OK;
}

int racs_pack_bool(msgpack_packer *pk, bool d) {
    msgpack_pack_array(pk, 2);
    racs_pack_type(pk, RACS_TYPE_BOOL);
    d ? msgpack_pack_true(pk) : msgpack_pack_false(pk);
    return RACS_STATUS_OK;
}

int racs_pack_s8v(msgpack_packer *pk, racs_int8 *data, size_t n) {
    msgpack_pack_array(pk, 2);

    racs_pack_type(pk, RACS_TYPE_S8VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(racs_int8));

    return RACS_STATUS_OK;
}

int racs_pack_u8v(msgpack_packer *pk, racs_uint8 *data, size_t n) {
    msgpack_pack_array(pk, 2);

    racs_pack_type(pk, RACS_TYPE_U8VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(racs_uint8));

    return RACS_STATUS_OK;
}

int racs_pack_s16v(msgpack_packer *pk, racs_int16 *data, size_t n) {
    msgpack_pack_array(pk, 2);

    racs_pack_type(pk, RACS_TYPE_S16VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(racs_int16));

    return RACS_STATUS_OK;
}

int racs_pack_u16v(msgpack_packer *pk, racs_uint16 *data, size_t n) {
    msgpack_pack_array(pk, 2);

    racs_pack_type(pk, RACS_TYPE_U16VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(racs_uint16));

    return RACS_STATUS_OK;
}

int racs_pack_s32v(msgpack_packer *pk, racs_int32 *data, size_t n) {
    msgpack_pack_array(pk, 2);

    racs_pack_type(pk, RACS_TYPE_S32VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(racs_int32));

    return RACS_STATUS_OK;
}

int racs_pack_u32v(msgpack_packer *pk, racs_uint32 *data, size_t n) {
    msgpack_pack_array(pk, 2);

    racs_pack_type(pk, RACS_TYPE_U32VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(racs_uint32));

    return RACS_STATUS_OK;
}

int racs_pack_f32v(msgpack_packer *pk, float *data, size_t n) {
    msgpack_pack_array(pk, 2);

    racs_pack_type(pk, RACS_TYPE_F32VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(float));

    return RACS_STATUS_OK;
}

int racs_pack_c64v(msgpack_packer *pk, racs_complex *data, size_t n) {
    msgpack_pack_array(pk, 2);

    racs_pack_type(pk, RACS_TYPE_C64VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(racs_complex));

    return RACS_STATUS_OK;
}

int racs_pack_invalid_num_args(msgpack_packer *pk, int expected, int actual) {
    char message[255];
    sprintf(message, "Expected %d args, but got %d", expected, actual);
    return racs_pack_error(pk, message);
}

int racs_is_object_type(msgpack_object *obj, msgpack_object_type type, int arg_num) {
    return obj->via.array.ptr[arg_num].type == type;
}

char *racs_unpack_str(msgpack_object *obj, int n) {
    size_t size = obj->via.array.ptr[n].via.str.size + 1;

    char *str = malloc(size);
    strlcpy(str, obj->via.array.ptr[n].via.str.ptr, size);

    return str;
}

int racs_pack_streams(msgpack_packer *pk, racs_streams *streams) {
    msgpack_pack_array(pk, streams->num_streams + 1);
    msgpack_pack_str_with_body(pk, "list", strlen("list"));

    for (int i = 0; i < streams->num_streams; ++i) {
        msgpack_pack_str_with_body(pk, streams->streams[i], strlen(streams->streams[i]));
    }

    return RACS_STATUS_OK;
}

racs_uint8 *racs_unpack_u8v(msgpack_object *obj, int n) {
    return (racs_uint8 *) obj->via.array.ptr[n].via.bin.ptr;
}

racs_int16 *racs_unpack_s16v(msgpack_object *obj, int n) {
    return (racs_int16 *) obj->via.array.ptr[n].via.bin.ptr;
}

racs_int32 *racs_unpack_s32v(msgpack_object *obj, int n) {
    return (racs_int32 *) obj->via.array.ptr[n].via.bin.ptr;
}

size_t racs_unpack_u8v_size(msgpack_object *obj, int n) {
    return obj->via.array.ptr[n].via.bin.size;
}

size_t racs_unpack_s16v_size(msgpack_object *obj, int n) {
    return obj->via.array.ptr[n].via.bin.size / sizeof(racs_int16);
}

size_t racs_unpack_s32v_size(msgpack_object *obj, int n) {
    return obj->via.array.ptr[n].via.bin.size / sizeof(racs_int32);
}

racs_uint16 racs_unpack_uint16(msgpack_object *obj, int n) {
    return (racs_uint16) obj->via.array.ptr[n].via.u64;
}

racs_int32 racs_unpack_int32(msgpack_object *obj, int n) {
    return (racs_int32) obj->via.array.ptr[n].via.i64;
}

racs_uint32 racs_unpack_uint32(msgpack_object *obj, int n) {
    return (racs_uint32) obj->via.array.ptr[n].via.u64;
}

racs_int64 racs_unpack_int64(msgpack_object *obj, int n) {
    return obj->via.array.ptr[n].via.i64;
}

racs_uint64 racs_unpack_uint64(msgpack_object *obj, int n) {
    return obj->via.array.ptr[n].via.u64;
}

float racs_unpack_float32(msgpack_object *obj, int n) {
    return (float)obj->via.array.ptr[n].via.f64;
}
