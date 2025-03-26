#include "commands.h"

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

    auxts_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR,
                            "Invalid type at arg 1 of CREATE command. Expected: string")

    auxts_validate_arg_type(&pk, msg, 1, MSGPACK_OBJECT_POSITIVE_INTEGER,
                            "Invalid type at arg 2 of CREATE command. Expected: int")

    auxts_validate_arg_type(&pk, msg, 2, MSGPACK_OBJECT_POSITIVE_INTEGER,
                            "Invalid type at arg 3 of CREATE command. Expected: int")

    auxts_validate_arg_type(&pk, msg, 3, MSGPACK_OBJECT_POSITIVE_INTEGER,
                            "Invalid type at arg 4 of CREATE command. Expected: int")

    char* stream_id = auxts_deserialize_str(&msg.data, 0);
    uint32_t sample_rate = auxts_deserialize_uint32(&msg.data, 1);
    uint32_t channels = auxts_deserialize_uint32(&msg.data, 2);
    uint32_t bit_depth = auxts_deserialize_uint32(&msg.data, 3);

    int rc = auxts_create(stream_id, sample_rate, channels, bit_depth);
    if (rc == AUXTS_METADATA_STATUS_OK)
        return auxts_serialize_null_with_status_ok(&pk);

    if (rc == AUXTS_METADATA_STATUS_EXIST)
        return auxts_serialize_error(&pk, "The stream-id already exist");

    if (rc == AUXTS_METADATA_STATUS_ERROR)
        return auxts_serialize_error(&pk, "Failed to create stream");

    return auxts_serialize_error(&pk, "Cause unknown");
}

auxts_create_command(eval) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    auxts_parse_args(in_buf, &pk)
    auxts_validate_num_args(&pk, msg, 1)

    auxts_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR,
                            "Invalid type at arg 1 of EVAL command. Expected string")

    char* error = NULL;
    char* expr = auxts_deserialize_str(&msg.data, 0);
    SCM res = auxts_scm_eval_with_error_handling(expr, &error);

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

    auxts_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR,
                             "Invalid type at arg 1 of EXTRACT command. Expected int or string")

    auxts_validate_arg_type(&pk, msg, 1, MSGPACK_OBJECT_POSITIVE_INTEGER,
                            "Invalid type at arg 2. Expected: time")

    auxts_validate_arg_type(&pk, msg, 2, MSGPACK_OBJECT_POSITIVE_INTEGER,
                            "Invalid type at arg 3. Expected: time")

    uint64_t stream_id;
    auxts_deserialize_stream_id(&stream_id, &msg.data, 0);

    int64_t from = auxts_deserialize_int64(&msg.data, 1);
    int64_t to = auxts_deserialize_int64(&msg.data, 2);


    auxts_pcm_buffer pbuf;
    int rc = auxts_extract_pcm_data(ctx->cache, &pbuf, stream_id, from, to);

    if (rc == AUXTS_EXTRACT_PCM_STATUS_OK)
        return auxts_serialize_pcm32(&pk, &pbuf);

    if (rc == AUXTS_EXTRACT_PCM_STATUS_NOT_FOUND)
        return auxts_serialize_null_with_status_not_found(&pk);

    return auxts_serialize_error(&pk, "Cause unknown");
}
