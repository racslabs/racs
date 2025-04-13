#include "command.h"

auxts_create_command(ping) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    auxts_parse_args(in_buf, &pk)
    auxts_validate_num_args(&pk, msg, 0)

    return auxts_serialize_str(&pk, "PONG");
}

auxts_create_command(create) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    auxts_parse_args(in_buf, &pk)

    auxts_validate_num_args(&pk, msg, 4)
    auxts_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected: string")
    auxts_validate_arg_type(&pk, msg, 1, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 2. Expected: int")
    auxts_validate_arg_type(&pk, msg, 2, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 3. Expected: int")
    auxts_validate_arg_type(&pk, msg, 3, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 4. Expected: int")

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

    auxts_parse_args(in_buf, &pk)

    auxts_validate_num_args(&pk, msg, 2)
    auxts_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")
    auxts_validate_arg_type(&pk, msg, 1, MSGPACK_OBJECT_STR, "Invalid type at arg 2. Expected string")

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

    auxts_parse_args(in_buf, &pk)

    auxts_validate_num_args(&pk, msg, 1)
    auxts_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")

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

    auxts_parse_args(in_buf, &pk)

    auxts_validate_num_args(&pk, msg, 3)
    auxts_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected: string")
    auxts_validate_arg_type(&pk, msg, 1, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 2. Expected: time")
    auxts_validate_arg_type(&pk, msg, 2, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 3. Expected: time")

    char* stream_id = auxts_deserialize_str(&msg.data, 0);
    int64_t from = auxts_deserialize_int64(&msg.data, 1);
    int64_t to = auxts_deserialize_int64(&msg.data, 2);

    auxts_pcm pcm;

    int rc = auxts_extract_pcm(ctx->cache, &pcm, stream_id, from, to);
    if (rc == AUXTS_EXTRACT_STATUS_NOT_FOUND) {
        free(pcm.out_stream.data);
        return auxts_serialize_error(&pk, "The stream-id does not exist");
    }

    if (pcm.bit_depth == AUXTS_PCM_16) {
        rc = auxts_serialize_i16v(&pk, (auxts_int16*)pcm.out_stream.data, pcm.samples * pcm.channels);
        free(pcm.out_stream.data);

        return rc;
    }

    if (pcm.bit_depth == AUXTS_PCM_24) {
        size_t size = pcm.channels * pcm.samples * sizeof(auxts_int32);
        auxts_int32* _out = malloc(size);

        auxts_simd_s24_s32((auxts_int24*)pcm.out_stream.data, _out, pcm.samples * pcm.channels);
        rc = auxts_serialize_i32v(&pk, _out, size);

        free(_out);
        free(pcm.out_stream.data);

        return rc;
    }

    return auxts_serialize_error(&pk, "Invalid bit-depth");
}
