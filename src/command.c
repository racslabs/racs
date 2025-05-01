#include "command.h"

rats_create_command(ping) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    rats_parse_buf(in_buf, &pk, &msg, "Error parsing args")
    rats_validate_num_args(&pk, msg, 0)

    return rats_serialize_str(&pk, "PONG");
}

rats_create_command(streamcreate) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    rats_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    rats_validate_num_args(&pk, msg, 3)
    rats_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected: string")
    rats_validate_type(&pk, msg, 1, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 2. Expected: int")
    rats_validate_type(&pk, msg, 2, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 3. Expected: int")

    char *stream_id = rats_deserialize_str(&msg.data, 0);

    rats_uint32 sample_rate = rats_deserialize_uint32(&msg.data, 1);
    rats_uint16 channels = rats_deserialize_uint16(&msg.data, 2);

    int rc = rats_streamcreate(ctx->mcache, stream_id, sample_rate, channels);
    free(stream_id);

    if (rc == 1)
        return rats_serialize_null_with_status_ok(&pk);

    return rats_serialize_error(&pk, "Failed to create stream");
}

rats_create_command(streamlist) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    rats_parse_buf(in_buf, &pk, &msg, "Error parsing args")
    rats_validate_num_args(&pk, msg, 1)
    rats_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")

    char *pattern = rats_deserialize_str(&msg.data, 0);

    rats_streams streams;
    rats_streams_init(&streams);
    rats_streaminfo_list(ctx->mcache, &streams, pattern);

    int rc = rats_serialize_streams(&pk, &streams);
    rats_streams_destroy(&streams);
    free(pattern);

    return rc;
}

rats_create_command(streamopen) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    rats_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    rats_validate_num_args(&pk, msg, 1)
    rats_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")

    char *stream_id = rats_deserialize_str(&msg.data, 0);
    int rc = rats_streamopen(ctx->kv, rats_hash(stream_id));
    free(stream_id);

    if (rc == 1)
        return rats_serialize_null_with_status_ok(&pk);

    return rats_serialize_error(&pk, "Stream is already open");
}

rats_create_command(streamclose) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    rats_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    rats_validate_num_args(&pk, msg, 1)
    rats_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")

    char *stream_id = rats_deserialize_str(&msg.data, 0);
    int rc = rats_streamclose(ctx->kv, rats_hash(stream_id));
    free(stream_id);

    if (rc == 1)
        return rats_serialize_null_with_status_ok(&pk);

    return rats_serialize_error(&pk, "Stream is not open");
}

rats_create_command(streaminfo) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    rats_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    rats_validate_num_args(&pk, msg, 2)
    rats_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")
    rats_validate_type(&pk, msg, 1, MSGPACK_OBJECT_STR, "Invalid type at arg 2. Expected string")

    char *stream_id = rats_deserialize_str(&msg.data, 0);
    char *attr = rats_deserialize_str(&msg.data, 1);

    rats_uint64 hash = rats_hash(stream_id);
    rats_uint64 value = rats_streaminfo_attr(ctx->mcache, hash, attr);

    free(stream_id);
    free(attr);

    if (value != 0) return rats_serialize_uint64(&pk, value);
    return rats_serialize_error(&pk, "Failure reading metadata");
}

rats_create_command(eval) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    rats_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    rats_validate_num_args(&pk, msg, 1)
    rats_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")

    char *error = NULL;
    char *expr = rats_deserialize_str(&msg.data, 0);
    SCM res = rats_scm_eval_with_error_handling(expr, &error);
    free(expr);

    if (error) return rats_serialize_error(&pk, error);

    return rats_scm_serialize(&pk, out_buf, res);
}

rats_create_command(extract) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    rats_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    rats_validate_num_args(&pk, msg, 3)
    rats_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected: string")
    rats_validate_type(&pk, msg, 1, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 2. Expected: time")
    rats_validate_type(&pk, msg, 2, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 3. Expected: time")

    char *stream_id = rats_deserialize_str(&msg.data, 0);
    int64_t from = rats_deserialize_int64(&msg.data, 1);
    int64_t to = rats_deserialize_int64(&msg.data, 2);

    rats_pcm pcm;

    int rc = rats_extract_pcm(ctx, &pcm, stream_id, from, to);
    if (rc == RATS_EXTRACT_STATUS_NOT_FOUND) {
        rats_pcm_destroy(&pcm);
        return rats_serialize_error(&pk, "The stream-id does not exist");
    }

    rc = rats_serialize_s16v(&pk, (rats_int16 *) pcm.out_stream.data, pcm.samples * pcm.channels);
    rats_pcm_destroy(&pcm);
    return rc;
}

rats_create_command(shutdown) {
    rats_context_destroy(ctx);
    exit(1);
}

rats_create_command(format) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg1;
    msgpack_unpacked_init(&msg1);

    msgpack_unpacked msg2;
    msgpack_unpacked_init(&msg2);

    rats_parse_buf(in_buf, &pk, &msg1, "Error parsing args")
    rats_parse_buf(out_buf, &pk, &msg2, "Error parsing buffer")

    rats_validate_type(&pk, msg1, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected: string")
    rats_validate_type(&pk, msg1, 1, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 2. Expected: int")
    rats_validate_type(&pk, msg1, 2, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 3. Expected: int")

    char *type = rats_deserialize_str(&msg2.data, 0);
    if (strcmp(type, "i16v") != 0) {
        free(type);
        msgpack_sbuffer_clear(out_buf);
        return rats_serialize_error(&pk, "Invalid input type. Expected: int16 array");
    }

    int16_t *in = rats_deserialize_s16v(&msg2.data, 1);
    size_t size = rats_deserialize_s16v_size(&msg2.data, 1);

    char *mime_type = rats_deserialize_str(&msg1.data, 0);
    rats_uint16 channels = rats_deserialize_uint16(&msg1.data, 1);
    rats_uint32 sample_rate = rats_deserialize_uint32(&msg1.data, 2);

    void *out = malloc(size * 2 + 44);

    rats_format fmt;
    fmt.channels = channels;
    fmt.sample_rate = sample_rate;

    size_t n = rats_format_pcm(&fmt, in, out, size / channels, size * 2 + 44, mime_type);

    if (n != 0) {
        msgpack_sbuffer_clear(out_buf);
        int rc = rats_serialize_u8v(&pk, out, n);

        free(type);
        free(mime_type);
        free(out);

        return rc;
    }

    free(type);
    free(mime_type);
    free(out);

    msgpack_sbuffer_clear(out_buf);
    return rats_serialize_error(&pk, "Unsupported format");
}

int rats_stream(msgpack_sbuffer *out_buf, rats_context *ctx, rats_uint8 *data) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    int rc = rats_streamappend(ctx->mcache, ctx->mmt, ctx->kv, data);
    if (rc == RATS_STREAM_OK)
        return rats_serialize_null_with_status_ok(&pk);

    return rats_serialize_error(&pk, rats_stream_status_string[rc]);
}