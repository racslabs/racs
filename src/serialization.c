#include "serialization.h"


const char* const auxts_type_string[] = {
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

void auxts_serialize_type(msgpack_packer* pk, int type) {
    const char* type_string = auxts_type_string[type];
    msgpack_pack_str_with_body(pk, type_string, strlen(type_string));
}

int auxts_serialize_null_with_status_ok(msgpack_packer* pk) {
    msgpack_pack_array(pk, 1);
    auxts_serialize_type(pk, AUXTS_TYPE_NULL);
    return AUXTS_STATUS_OK;
}

int auxts_serialize_null_with_status_not_found(msgpack_packer* pk) {
    msgpack_pack_array(pk, 1);
    auxts_serialize_type(pk, AUXTS_TYPE_NULL);
    return AUXTS_STATUS_NOT_FOUND;
}

int auxts_serialize_error(msgpack_packer* pk, const char* message) {
    msgpack_pack_array(pk, 2);
    auxts_serialize_type(pk, AUXTS_TYPE_ERROR);
    msgpack_pack_str_with_body(pk, message, strlen(message));
    return AUXTS_STATUS_ERROR;
}

int auxts_serialize_str(msgpack_packer* pk, const char* str) {
    msgpack_pack_array(pk, 2);
    auxts_serialize_type(pk, AUXTS_TYPE_STR);
    msgpack_pack_str_with_body(pk, str, strlen(str));
    return AUXTS_STATUS_OK;
}

int auxts_serialize_int64(msgpack_packer* pk, int64_t d) {
    msgpack_pack_array(pk, 2);
    auxts_serialize_type(pk, AUXTS_TYPE_INT);
    msgpack_pack_int64(pk, d);
    return AUXTS_STATUS_OK;
}

int auxts_serialize_uint64(msgpack_packer* pk, uint64_t d) {
    msgpack_pack_array(pk, 2);
    auxts_serialize_type(pk, AUXTS_TYPE_INT);
    msgpack_pack_uint64(pk, d);
    return AUXTS_STATUS_OK;
}

int auxts_serialize_float64(msgpack_packer* pk, double d) {
    msgpack_pack_array(pk, 2);
    auxts_serialize_type(pk, AUXTS_TYPE_FLOAT);
    msgpack_pack_double(pk, d);
    return AUXTS_STATUS_OK;
}

int auxts_serialize_bool(msgpack_packer* pk, bool d) {
    msgpack_pack_array(pk, 2);
    auxts_serialize_type(pk, AUXTS_TYPE_BOOL);
    d ? msgpack_pack_true(pk) : msgpack_pack_false(pk);
    return AUXTS_STATUS_OK;
}

int auxts_serialize_pcm32(msgpack_packer* pk, const auxts_pcm_buffer* pbuf) {
    int32_t* data = auxts_flatten_pcm_data(pbuf);
    auxts_serialize_i32v(pk, data, pbuf->info.num_samples * pbuf->info.channels);
    free(data);

    return AUXTS_STATUS_OK;
}

int auxts_serialize_i8v(msgpack_packer* pk, int8_t* data, size_t n) {
    msgpack_pack_array(pk, 2);

    auxts_serialize_type(pk, AUXTS_TYPE_I8VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(int8_t));

    return AUXTS_STATUS_OK;
}

int auxts_serialize_u8v(msgpack_packer* pk, uint8_t* data, size_t n) {
    msgpack_pack_array(pk, 2);

    auxts_serialize_type(pk, AUXTS_TYPE_U8VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(uint8_t));

    return AUXTS_STATUS_OK;
}

int auxts_serialize_i16v(msgpack_packer* pk, int16_t* data, size_t n) {
    msgpack_pack_array(pk, 2);

    auxts_serialize_type(pk, AUXTS_TYPE_I16VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(int16_t));

    return AUXTS_STATUS_OK;
}

int auxts_serialize_u16v(msgpack_packer* pk, uint16_t* data, size_t n) {
    msgpack_pack_array(pk, 2);

    auxts_serialize_type(pk, AUXTS_TYPE_U16VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(uint16_t));

    return AUXTS_STATUS_OK;
}

int auxts_serialize_i32v(msgpack_packer* pk, int32_t* data, size_t n) {
    msgpack_pack_array(pk, 2);

    auxts_serialize_type(pk, AUXTS_TYPE_I32VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(int32_t));

    return AUXTS_STATUS_OK;
}

int auxts_serialize_u32v(msgpack_packer* pk, uint32_t* data, size_t n) {
    msgpack_pack_array(pk, 2);

    auxts_serialize_type(pk, AUXTS_TYPE_U32VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(uint32_t));

    return AUXTS_STATUS_OK;
}

int auxts_serialize_f32v(msgpack_packer* pk, float* data, size_t n) {
    msgpack_pack_array(pk, 2);

    auxts_serialize_type(pk, AUXTS_TYPE_F32VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(float));

    return AUXTS_STATUS_OK;
}

int auxts_serialize_c64v(msgpack_packer* pk, auxts_complex_t* data, size_t n) {
    msgpack_pack_array(pk, 2);

    auxts_serialize_type(pk, AUXTS_TYPE_C64VEC);
    msgpack_pack_bin_with_body(pk, data, n * sizeof(auxts_complex_t));

    return AUXTS_STATUS_OK;
}

int auxts_serialize_invalid_num_args(msgpack_packer* pk, int expected, int actual) {
    char message[255];
    sprintf(message, "Expected %d args, but got %d", expected, actual);
    return auxts_serialize_error(pk, message);
}

void auxts_deserialize_stream_id(uint64_t* stream_id, msgpack_object* obj, int arg_num) {
    char* str = auxts_deserialize_str(obj, arg_num);

    uint64_t hash[2];
    murmur3_x64_128((uint8_t*)str, strlen(str), 0, hash);
    *stream_id = hash[0];

    free(str);
}

int auxts_is_object_type(msgpack_object* obj, msgpack_object_type type, int arg_num) {
    return obj->via.array.ptr[arg_num].type == type;
}

char* auxts_deserialize_str(msgpack_object* obj, int n) {
    size_t size = obj->via.array.ptr[n].via.str.size + 1;

    char* str = malloc(size);
    strlcpy(str, obj->via.array.ptr[n].via.str.ptr, size);

    return str;
}

int32_t* auxts_deserialize_i32v(msgpack_object* obj, int n) {
    return (int32_t*)obj->via.array.ptr[n].via.bin.ptr;
}

size_t auxts_deserialize_i32v_size(msgpack_object* obj, int n) {
    return obj->via.array.ptr[n].via.bin.size / sizeof(int32_t);
}

int32_t auxts_deserialize_int32(msgpack_object* obj, int n) {
    return (int32_t)obj->via.array.ptr[n].via.i64;
}

uint32_t auxts_deserialize_uint32(msgpack_object* obj, int n) {
    return (uint32_t)obj->via.array.ptr[n].via.u64;
}

int64_t auxts_deserialize_int64(msgpack_object* obj, int n) {
    return obj->via.array.ptr[n].via.i64;
}

uint64_t auxts_deserialize_uint64(msgpack_object* obj, int n) {
    return obj->via.array.ptr[n].via.u64;
}