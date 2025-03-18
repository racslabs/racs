#include "serialization.h"


const char* const auxts_status_code[] = {
        "OK",
        "NOT_FOUND",
        "ERROR"
};

static void serialize_pcm_data(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);
static void serialize_pcm_bit_depth(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);
static void serialize_pcm_sample_rate(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);
static void serialize_pcm_channels(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);
static void serialize_pcm_samples(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);

int auxts_serialize_status_ok(msgpack_packer* pk) {
    return auxts_serialize_status(pk, AUXTS_STATUS_OK);
}

int auxts_serialize_status_not_found(msgpack_packer* pk) {
    msgpack_pack_array(pk, 2);
    return auxts_serialize_status(pk, AUXTS_STATUS_NOT_FOUND);
}

int auxts_serialize_pcm_buffer(msgpack_packer* pk, auxts_pcm_buffer* pbuf) {
    msgpack_pack_array(pk, 12);

    auxts_serialize_status_ok(pk);
    serialize_pcm_samples(pk, pbuf);
    serialize_pcm_channels(pk, pbuf);
    serialize_pcm_sample_rate(pk, pbuf);
    serialize_pcm_bit_depth(pk, pbuf);
    serialize_pcm_data(pk, pbuf);

    auxts_pcm_buffer_destroy(pbuf);
    return AUXTS_STATUS_OK;
}

void serialize_pcm_samples(msgpack_packer* pk, const auxts_pcm_buffer* pbuf) {
    msgpack_pack_str_with_body(pk, "samples", strlen("samples"));
    msgpack_pack_uint32(pk, pbuf->info.num_samples);
}

void serialize_pcm_channels(msgpack_packer* pk, const auxts_pcm_buffer* pbuf) {
    msgpack_pack_str_with_body(pk, "channels", strlen("channels"));
    msgpack_pack_uint32(pk, pbuf->info.channels);
}

void serialize_pcm_sample_rate(msgpack_packer* pk, const auxts_pcm_buffer* pbuf) {
    msgpack_pack_str_with_body(pk, "sample_rate", strlen("sample_rate"));
    msgpack_pack_uint32(pk, pbuf->info.sample_rate);
}

void serialize_pcm_bit_depth(msgpack_packer* pk, const auxts_pcm_buffer* pbuf) {
    msgpack_pack_str_with_body(pk, "bit_depth", strlen("bit_depth"));
    msgpack_pack_uint32(pk, pbuf->info.bit_depth);
}

int auxts_serialize_status(msgpack_packer* pk, int status) {
    const char* status_code = auxts_status_code[status];
    msgpack_pack_str_with_body(pk, "status", strlen("status"));
    msgpack_pack_str_with_body(pk, status_code, strlen(status_code));
    return status;
}

int auxts_serialize_status_error(msgpack_packer* pk, const char* message) {
    msgpack_pack_array(pk, 4);
    auxts_serialize_status(pk, AUXTS_STATUS_ERROR);
    auxts_serialize_message(pk, message);
    return AUXTS_STATUS_ERROR;
}

void auxts_serialize_message(msgpack_packer* pk, const char* message) {
    msgpack_pack_str_with_body(pk, "message", strlen("message"));
    msgpack_pack_str_with_body(pk, message, strlen(message));
}

void serialize_pcm_data(msgpack_packer* pk, const auxts_pcm_buffer* pbuf) {
    uint8_t* data = auxts_pack_pcm_data(pbuf);
    size_t size = pbuf->info.num_samples * pbuf->info.channels * sizeof(int32_t);

    msgpack_pack_str_with_body(pk, "pcm_data", strlen("pcm_data"));
    msgpack_pack_bin_with_body(pk, data, size);

    free(data);
}

void auxts_deserialize_stream_id(uint64_t* stream_id, msgpack_object* obj, int arg_num) {
    if (auxts_is_object_type(obj, MSGPACK_OBJECT_NEGATIVE_INTEGER, arg_num)) {
        *stream_id = auxts_deserialize_uint64(obj, arg_num);
        printf("des %llu\n", *stream_id);
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

int auxts_serialize_invalid_num_args(msgpack_packer* pk, int expected, int actual) {
    char message[255];
    sprintf(message, "Expected %d args, but got %d", expected, actual);
    return auxts_serialize_status_error(pk, message);
}
