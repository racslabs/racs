#include "command.h"

auxts_create_command(ping) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    auxts_parse_buf(in_buf, &pk, &msg, "Error parsing args")
    auxts_validate_num_args(&pk, msg, 0)

    return auxts_serialize_str(&pk, "PONG");
}

auxts_create_command(create) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    auxts_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    auxts_validate_num_args(&pk, msg, 4)
    auxts_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected: string")
    auxts_validate_type(&pk, msg, 1, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 2. Expected: int")
    auxts_validate_type(&pk, msg, 2, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 3. Expected: int")
    auxts_validate_type(&pk, msg, 3, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 4. Expected: int")

    char* stream_id = auxts_deserialize_str(&msg.data, 0);
    uint32_t sample_rate = auxts_deserialize_uint32(&msg.data, 1);
    uint32_t channels = auxts_deserialize_uint32(&msg.data, 2);
    uint32_t bit_depth = auxts_deserialize_uint32(&msg.data, 3);

    int rc = auxts_create(stream_id, sample_rate, channels, bit_depth);
    free(stream_id);

    if (rc == AUXTS_METADATA_STATUS_OK)
        return auxts_serialize_null_with_status_ok(&pk);

    if (rc == AUXTS_METADATA_STATUS_EXIST)
        return auxts_serialize_error(&pk, "The stream-id already exist");

    return auxts_serialize_error(&pk, "Failed to create stream");
}

auxts_create_command(metadata) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    auxts_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    auxts_validate_num_args(&pk, msg, 2)
    auxts_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")
    auxts_validate_type(&pk, msg, 1, MSGPACK_OBJECT_STR, "Invalid type at arg 2. Expected string")

    char* stream_id = auxts_deserialize_str(&msg.data, 0);
    char* attr = auxts_deserialize_str(&msg.data, 1);

    uint64_t value;
    int rc = auxts_metadata_attr(stream_id, attr, &value);

    free(stream_id);
    free(attr);

    if (rc == AUXTS_METADATA_STATUS_OK) {
        return auxts_serialize_uint64(&pk, value);
    }

    if (rc == AUXTS_METADATA_STATUS_NOT_FOUND) {
        return auxts_serialize_null_with_status_not_found(&pk);
    }

    return auxts_serialize_error(&pk, "Failure reading metadata");
}

auxts_create_command(eval) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    auxts_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    auxts_validate_num_args(&pk, msg, 1)
    auxts_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")

    char* error = NULL;
    char* expr = auxts_deserialize_str(&msg.data, 0);
    SCM res = auxts_scm_eval_with_error_handling(expr, &error);
    free(expr);

    if (error) return auxts_serialize_error(&pk, error);

    return auxts_scm_serialize(&pk, out_buf, res);
}

auxts_create_command(extract) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    auxts_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    auxts_validate_num_args(&pk, msg, 3)
    auxts_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected: string")
    auxts_validate_type(&pk, msg, 1, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 2. Expected: time")
    auxts_validate_type(&pk, msg, 2, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 3. Expected: time")

    char* stream_id = auxts_deserialize_str(&msg.data, 0);
    int64_t from = auxts_deserialize_int64(&msg.data, 1);
    int64_t to = auxts_deserialize_int64(&msg.data, 2);

    auxts_pcm pcm;

    int rc = auxts_extract_pcm(ctx->cache, &pcm, stream_id, from, to);
    if (rc == AUXTS_EXTRACT_STATUS_NOT_FOUND) {
        auxts_pcm_destroy(&pcm);
        return auxts_serialize_error(&pk, "The stream-id does not exist");
    }

    rc = auxts_serialize_i16v(&pk, (auxts_int16*)pcm.out_stream.data, pcm.samples * pcm.channels);
    auxts_pcm_destroy(&pcm);
    return rc;
}

auxts_create_command(format) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg1;
    msgpack_unpacked_init(&msg1);

    msgpack_unpacked msg2;
    msgpack_unpacked_init(&msg2);

    auxts_parse_buf(in_buf, &pk, &msg1, "Error parsing args")
    auxts_parse_buf(out_buf, &pk, &msg2, "Error parsing buffer")

    auxts_validate_type(&pk, msg1, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected: string")
    auxts_validate_type(&pk, msg1, 1, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 2. Expected: int")
    auxts_validate_type(&pk, msg1, 2, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 3. Expected: int")

    char* type = auxts_deserialize_str(&msg2.data, 0);
    if (strcmp(type, "i16v") != 0) {
        free(type);
        msgpack_sbuffer_clear(out_buf);
        return auxts_serialize_error(&pk, "Invalid input type. Expected: int16 array");
    }

    int16_t* in = auxts_deserialize_i16v(&msg2.data, 1);
    size_t size = auxts_deserialize_i16v_size(&msg2.data, 1);

    char* mime_type = auxts_deserialize_str(&msg1.data, 0);
    uint16_t channels = auxts_deserialize_uint16(&msg1.data, 1);
    uint32_t sample_rate = auxts_deserialize_uint32(&msg1.data, 2);

    void* out = malloc(size * 2 + 44);

    auxts_format fmt;
    fmt.channels = channels;
    fmt.sample_rate = sample_rate;

    size_t n = auxts_format_pcm(&fmt, in, out, size / channels, size * 2 + 44, mime_type);

    if (n != 0) {
        msgpack_sbuffer_clear(out_buf);
        int rc = auxts_serialize_u8v(&pk, out, n);

        free(type);
        free(mime_type);
        free(out);

        return rc;
    }

    free(type);
    free(mime_type);
    free(out);

    msgpack_sbuffer_clear(out_buf);
    return auxts_serialize_error(&pk, "Unsupported format");
}