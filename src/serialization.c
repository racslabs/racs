#include "serialization.h"


const char* const auxts_status_code[] = {
        "OK",
        "NO_DATA",
        "ERROR"
};

static void serialize_pcm_data(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);
static void serialize_pcm_bit_depth(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);
static void serialize_pcm_sample_rate(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);
static void serialize_pcm_channels(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);
static void serialize_pcm_samples(msgpack_packer* pk, const auxts_pcm_buffer* pbuf);

void auxts_serialize_status_ok(msgpack_packer* pk) {
    auxts_serialize_status(pk, 0);
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
    return AUXTS_COMMAND_STATUS_OK;
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

void auxts_serialize_status(msgpack_packer* pk, int status) {
    const char* status_code = auxts_status_code[status];
    msgpack_pack_str_with_body(pk, "status", strlen("status"));
    msgpack_pack_str_with_body(pk, status_code, strlen(status_code));
}

int auxts_serialize_status_not_ok(msgpack_packer* pk, int status, const char* message) {
    msgpack_pack_array(pk, 4);
    auxts_serialize_status(pk, status);
    auxts_serialize_message(pk, message);
    return status;
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

void auxts_deserialize_stream_id(uint64_t* stream_id, msgpack_object* obj) {
    size_t size = obj->via.array.ptr[0].via.str.size + 1;

    char* buf = malloc(size);
    strlcpy(buf, obj->via.array.ptr[0].via.str.ptr, size);

    uint64_t hash[2];
    murmur3_x64_128((uint8_t*)buf, strlen(buf), 0, hash);
    *stream_id = hash[0];

    free(buf);
}

void auxts_deserialize_from(int64_t* from, msgpack_object* obj) {
    char buf[55];

    size_t size = obj->via.array.ptr[1].via.str.size + 1;
    strlcpy(buf, obj->via.array.ptr[1].via.str.ptr, size);
    *from = auxts_parse_rfc3339(buf);
}

void auxts_deserialize_to(int64_t* to, msgpack_object* obj) {
    char buf[55];

    size_t size = obj->via.array.ptr[2].via.str.size + 1;
    strlcpy(buf, obj->via.array.ptr[2].via.str.ptr, size);
    *to = auxts_parse_rfc3339(buf);
}

int auxts_deserialize_range(int64_t* from, int64_t* to, msgpack_object* obj) {
    auxts_deserialize_from(from, obj);
    auxts_deserialize_to(to, obj);
    return *from == -1 || *to == -1;
}

void auxts_serialize_invalid_num_args(msgpack_packer* pk, int expected, int actual) {
    char message[255];
    sprintf(message, "Expected %d args, but got %d", expected, actual);
    auxts_serialize_status_not_ok(pk, AUXTS_COMMAND_STATUS_ERROR, message);
}
