#include "serialization.h"


const char* const rats_type_string[] = {
        "string",
        "int",
        "float",
        "list",
        "null",
        "error",
        "bool",
        "u8v",
        "i8v",
        "u16v",
        "i16v",
        "u32v",
        "i32v",
        "f32v",
        "c64v"
};

void rats_serialize_type(msgpack_packer* pk, int type) {
    const char* type_string = rats_type_string[type];
    msgpack_pack_str_with_body(pk, type_string, strlen(type_string));
}

int rats_serialize_null_with_status_ok(msgpack_packer* pk) {
    msgpack_pack_array(pk, 1);
    rats_serialize_type(pk, RATS_TYPE_NULL);
    return RATS_STATUS_OK;
}

int rats_serialize_null_with_status_not_found(msgpack_packer* pk) {
    msgpack_pack_array(pk, 1);
    rats_serialize_type(pk, RATS_TYPE_NULL);
    return RATS_STATUS_NOT_FOUND;
}

int rats_serialize_error(msgpack_packer* pk, const char* message) {
    msgpack_pack_array(pk, 2);
    rats_serialize_type(pk, RATS_TYPE_ERROR);
    msgpack_pack_str_with_body(pk, message, strlen(message));
    return RATS_STATUS_ERROR;
}

int rats_serialize_str(msgpack_packer* pk, const char* str) {
    msgpack_pack_array(pk, 2);
    rats_serialize_type(pk, RATS_TYPE_STR);
    msgpack_pack_str_with_body(pk, str, strlen(str));
    return RATS_STATUS_OK;
}

int rats_serialize_int64(msgpack_packer* pk, rats_int64 d) {
    msgpack_pack_array(pk, 2);
    rats_serialize_type(pk, RATS_TYPE_INT);
    msgpack_pack_int64(pk, d);
    return RATS_STATUS_OK;
}

int rats_serialize_uint64(msgpack_packer* pk, rats_uint64 d) {
    msgpack_pack_array(pk, 2);
    rats_serialize_type(pk, RATS_TYPE_INT);
    msgpack_pack_uint64(pk, d);
    return RATS_STATUS_OK;
}

int rats_serialize_float64(msgpack_packer* pk, double d) {
    msgpack_pack_array(pk, 2);
    rats_serialize_type(pk, RATS_TYPE_FLOAT);
    msgpack_pack_double(pk, d);
    return RATS_STATUS_OK;
}

int rats_serialize_bool(msgpack_packer* pk, bool d) {
    msgpack_pack_array(pk, 2);
    rats_serialize_type(pk, RATS_TYPE_BOOL);
    d ? msgpack_pack_true(pk) : msgpack_pack_false(pk);
    return RATS_STATUS_OK;
}

int rats_serialize_s8v(msgpack_packer* pk, rats_int8* data, size_t n) {
    msgpack_pack_array(pk, 2);

    rats_serialize_type(pk, RATS_TYPE_I8VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(rats_int8));

    return RATS_STATUS_OK;
}

int rats_serialize_u8v(msgpack_packer* pk, rats_uint8* data, size_t n) {
    msgpack_pack_array(pk, 2);

    rats_serialize_type(pk, RATS_TYPE_U8VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(rats_uint8));

    return RATS_STATUS_OK;
}

int rats_serialize_s16v(msgpack_packer* pk, rats_int16* data, size_t n) {
    msgpack_pack_array(pk, 2);

    rats_serialize_type(pk, RATS_TYPE_I16VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(rats_int16));

    return RATS_STATUS_OK;
}

int rats_serialize_u16v(msgpack_packer* pk, rats_uint16* data, size_t n) {
    msgpack_pack_array(pk, 2);

    rats_serialize_type(pk, RATS_TYPE_U16VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(rats_uint16));

    return RATS_STATUS_OK;
}

int rats_serialize_s32v(msgpack_packer* pk, rats_int32* data, size_t n) {
    msgpack_pack_array(pk, 2);

    rats_serialize_type(pk, RATS_TYPE_I32VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(rats_int32));

    return RATS_STATUS_OK;
}

int rats_serialize_u32v(msgpack_packer* pk, rats_uint32* data, size_t n) {
    msgpack_pack_array(pk, 2);

    rats_serialize_type(pk, RATS_TYPE_U32VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(rats_uint32));

    return RATS_STATUS_OK;
}

int rats_serialize_f32v(msgpack_packer* pk, float* data, size_t n) {
    msgpack_pack_array(pk, 2);

    rats_serialize_type(pk, RATS_TYPE_F32VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(float));

    return RATS_STATUS_OK;
}

int rats_serialize_c64v(msgpack_packer* pk, rats_complex* data, size_t n) {
    msgpack_pack_array(pk, 2);

    rats_serialize_type(pk, RATS_TYPE_C64VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(rats_complex));

    return RATS_STATUS_OK;
}

int rats_serialize_invalid_num_args(msgpack_packer* pk, int expected, int actual) {
    char message[255];
    sprintf(message, "Expected %d args, but got %d", expected, actual);
    return rats_serialize_error(pk, message);
}

int rats_is_object_type(msgpack_object* obj, msgpack_object_type type, int arg_num) {
    return obj->via.array.ptr[arg_num].type == type;
}

char* rats_deserialize_str(msgpack_object* obj, int n) {
    size_t size = obj->via.array.ptr[n].via.str.size + 1;

    char* str = malloc(size);
    strlcpy(str, obj->via.array.ptr[n].via.str.ptr, size);

    return str;
}

rats_uint8* rats_deserialize_u8v(msgpack_object* obj, int n) {
    return (rats_uint8*)obj->via.array.ptr[n].via.bin.ptr;
}

rats_int16* rats_deserialize_s16v(msgpack_object* obj, int n) {
    return (rats_int16*)obj->via.array.ptr[n].via.bin.ptr;
}

rats_int32* rats_deserialize_s32v(msgpack_object* obj, int n) {
    return (rats_int32*)obj->via.array.ptr[n].via.bin.ptr;
}

size_t rats_deserialize_u8v_size(msgpack_object* obj, int n) {
    return obj->via.array.ptr[n].via.bin.size;
}

size_t rats_deserialize_s16v_size(msgpack_object* obj, int n) {
    return obj->via.array.ptr[n].via.bin.size / sizeof(rats_int16);
}

size_t rats_deserialize_s32v_size(msgpack_object* obj, int n) {
    return obj->via.array.ptr[n].via.bin.size / sizeof(rats_int32);
}

rats_uint16 rats_deserialize_uint16(msgpack_object* obj, int n) {
    return (rats_uint16)obj->via.array.ptr[n].via.u64;
}

rats_int32 rats_deserialize_int32(msgpack_object* obj, int n) {
    return (rats_int32)obj->via.array.ptr[n].via.i64;
}

rats_uint32 rats_deserialize_uint32(msgpack_object* obj, int n) {
    return (rats_uint32)obj->via.array.ptr[n].via.u64;
}

rats_int64 rats_deserialize_int64(msgpack_object* obj, int n) {
    return obj->via.array.ptr[n].via.i64;
}

rats_uint64 rats_deserialize_uint64(msgpack_object* obj, int n) {
    return obj->via.array.ptr[n].via.u64;
}