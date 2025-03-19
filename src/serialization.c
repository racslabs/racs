#include "serialization.h"


const char* const auxts_type_string[] = {
        "str",
        "bin",
        "int",
        "float",
        "map",
        "list",
        "none",
        "error"
};

void auxts_serialize_type(msgpack_packer* pk, int type) {
    const char* type_string = auxts_type_string[type];
    msgpack_pack_str_with_body(pk, type_string, strlen(type_string));
}

int auxts_serialize_none_with_status_ok(msgpack_packer* pk) {
    msgpack_pack_array(pk, 1);
    auxts_serialize_type(pk, AUXTS_TYPE_NONE);
    return AUXTS_STATUS_OK;
}

int auxts_serialize_none_with_status_not_found(msgpack_packer* pk) {
    msgpack_pack_array(pk, 1);
    auxts_serialize_type(pk, AUXTS_TYPE_NONE);
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

int auxts_serialize_bin(msgpack_packer* pk, const uint8_t* data, size_t n) {
    msgpack_pack_array(pk, 2);
    auxts_serialize_type(pk, AUXTS_TYPE_BIN);
    msgpack_pack_bin_with_body(pk, data, n);
    return AUXTS_STATUS_OK;
}

int auxts_serialize_int64(msgpack_packer* pk, int64_t d) {
    msgpack_pack_array(pk, 2);
    auxts_serialize_type(pk, AUXTS_TYPE_INT);
    msgpack_pack_int64(pk, d);
    return AUXTS_STATUS_OK;
}

int auxts_serialize_int32(msgpack_packer* pk, float d) {
    msgpack_pack_array(pk, 2);
    auxts_serialize_type(pk, AUXTS_TYPE_FLOAT);
    msgpack_pack_float(pk, d);
    return AUXTS_STATUS_OK;
}

int auxts_serialize_invalid_num_args(msgpack_packer* pk, int expected, int actual) {
    char message[255];
    sprintf(message, "Expected %d args, but got %d", expected, actual);
    return auxts_serialize_error(pk, message);
}

int auxts_serialize_pcm32(msgpack_packer* pk, const auxts_pcm_buffer* pbuf) {
    uint8_t* data = auxts_pack_pcm_data(pbuf);
    size_t size = pbuf->info.num_samples * pbuf->info.channels * sizeof(int32_t);
    auxts_serialize_bin(pk, data, size);
    free(data);

    return AUXTS_STATUS_OK;
}

void auxts_deserialize_stream_id(uint64_t* stream_id, msgpack_object* obj, int arg_num) {
    if (auxts_is_object_type(obj, MSGPACK_OBJECT_NEGATIVE_INTEGER, arg_num)) {
        *stream_id = auxts_deserialize_uint64(obj, arg_num);
    }

    if (auxts_is_object_type(obj, MSGPACK_OBJECT_STR, arg_num)) {
        char* str = auxts_deserialize_str(obj, arg_num);

        uint64_t hash[2];
        murmur3_x64_128((uint8_t*)str, strlen(str), 0, hash);
        *stream_id = hash[0];

        free(str);
    }

}

int auxts_is_object_type(msgpack_object* obj, msgpack_object_type type, int arg_num) {
    return obj->via.array.ptr[arg_num].type == type;
}

char* auxts_deserialize_str(msgpack_object* obj, int arg_num) {
    size_t size = obj->via.array.ptr[arg_num].via.str.size + 1;

    char* str = malloc(size);
    strlcpy(str, obj->via.array.ptr[arg_num].via.str.ptr, size);

    return str;
}

int32_t auxts_deserialize_int32(msgpack_object* obj, int arg_num) {
    return (int32_t)obj->via.array.ptr[arg_num].via.i64;
}

uint32_t auxts_deserialize_uint32(msgpack_object* obj, int arg_num) {
    return (uint32_t)obj->via.array.ptr[arg_num].via.u64;
}

uint64_t auxts_deserialize_uint64(msgpack_object* obj, int arg_num) {
    return obj->via.array.ptr[arg_num].via.i64;
}

void auxts_deserialize_from(int64_t* from, msgpack_object* obj) {
    char* str = auxts_deserialize_str(obj, 1);
    *from = auxts_parse_rfc3339(str);
    free(str);
}

void auxts_deserialize_to(int64_t* to, msgpack_object* obj) {
    char* str = auxts_deserialize_str(obj, 2);
    *to = auxts_parse_rfc3339(str);
    free(str);
}

int auxts_deserialize_range(int64_t* from, int64_t* to, msgpack_object* obj) {
    auxts_deserialize_from(from, obj);
    auxts_deserialize_to(to, obj);
    return *from != -1 && *to != -1;
}
