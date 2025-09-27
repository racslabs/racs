#include "command.h"

racs_create_command(ping) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "Error parsing args")
    racs_validate_num_args(&pk, msg, 0)

    return racs_pack_str(&pk, "PONG");
}

racs_create_command(streamcreate) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    racs_validate_num_args(&pk, msg, 4)
    racs_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected: string")
    racs_validate_type(&pk, msg, 1, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 2. Expected: int")
    racs_validate_type(&pk, msg, 2, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 3. Expected: int")
    racs_validate_type(&pk, msg, 3, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 4. Expected: int")

    char *stream_id = racs_unpack_str(&msg.data, 0);

    racs_uint32 sample_rate = racs_unpack_uint32(&msg.data, 1);
    racs_uint16 channels = racs_unpack_uint16(&msg.data, 2);
    racs_uint16 bit_depth = racs_unpack_uint16(&msg.data, 3);

    int rc = racs_streamcreate(ctx->mcache, stream_id, sample_rate, channels, bit_depth);
    free(stream_id);

    if (rc == 1)
        return racs_pack_null_with_status_ok(&pk);

    return racs_pack_error(&pk, "Failed to create stream");
}

racs_create_command(streamlist) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "Error parsing args")
    racs_validate_num_args(&pk, msg, 1)
    racs_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")

    char *pattern = racs_unpack_str(&msg.data, 0);

    racs_streams streams;
    racs_streams_init(&streams);
    racs_streaminfo_list(ctx->mcache, &streams, pattern);

    int rc = racs_pack_streams(&pk, &streams);
    racs_streams_destroy(&streams);
    free(pattern);

    return rc;
}

racs_create_command(streamopen) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    racs_validate_num_args(&pk, msg, 1)
    racs_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")

    char *stream_id = racs_unpack_str(&msg.data, 0);
    int rc = racs_streamopen(ctx->kv, racs_hash(stream_id));
    free(stream_id);

    if (rc == 1)
        return racs_pack_null_with_status_ok(&pk);

    return racs_pack_error(&pk, "Stream is already open");
}

racs_create_command(streamclose) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    racs_validate_num_args(&pk, msg, 1)
    racs_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")

    char *stream_id = racs_unpack_str(&msg.data, 0);
    int rc = racs_streamclose(ctx->kv, racs_hash(stream_id));
    free(stream_id);

    if (rc == 1)
        return racs_pack_null_with_status_ok(&pk);

    return racs_pack_error(&pk, "Stream is not open");
}

racs_create_command(streaminfo) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    racs_validate_num_args(&pk, msg, 2)
    racs_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")
    racs_validate_type(&pk, msg, 1, MSGPACK_OBJECT_STR, "Invalid type at arg 2. Expected string")

    char *stream_id = racs_unpack_str(&msg.data, 0);
    char *attr = racs_unpack_str(&msg.data, 1);

    racs_uint64 hash = racs_hash(stream_id);
    racs_int64 value = racs_streaminfo_attr(ctx->mcache, hash, attr);

    free(stream_id);
    free(attr);

    if (value != 0) return racs_pack_int64(&pk, value);
    return racs_pack_error(&pk, "Failure reading metadata");
}

racs_create_command(eval) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    racs_validate_num_args(&pk, msg, 1)
    racs_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected string")

    char *error = NULL;
    char *expr = racs_unpack_str(&msg.data, 0);
    SCM res = racs_scm_eval_with_error_handling(expr, &error);
    free(expr);

    if (error) return racs_pack_error(&pk, error);

    return racs_scm_pack(&pk, out_buf, res);
}

racs_create_command(extract) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "Error parsing args")

    racs_validate_num_args(&pk, msg, 3)
    racs_validate_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected: string")
    racs_validate_type(&pk, msg, 1, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 2. Expected: time")
    racs_validate_type(&pk, msg, 2, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 3. Expected: time")

    char *stream_id = racs_unpack_str(&msg.data, 0);
    int64_t from = racs_unpack_int64(&msg.data, 1);
    int64_t to = racs_unpack_int64(&msg.data, 2);

    racs_pcm pcm;

    int rc = racs_extract_pcm(ctx, &pcm, stream_id, from, to);
    if (rc == RACS_EXTRACT_STATUS_NOT_FOUND) {
        racs_pcm_destroy(&pcm);
        return racs_pack_error(&pk, "The stream-id does not exist");
    }

    racs_int32 *out = NULL;

    if (pcm.bit_depth == 16)
        out = racs_s16_s32((const racs_int16 *) pcm.out_stream.data, pcm.samples * pcm.channels);
    if (pcm.bit_depth == 24)
        out = racs_s24_s32((const racs_int24 *) pcm.out_stream.data, pcm.samples * pcm.channels);

    rc = racs_pack_s32v(&pk, out, pcm.samples * pcm.channels);

    free(pcm.out_stream.data);
    free(out);

    return rc;
}

racs_create_command(shutdown) {
    racs_context_destroy(ctx);
    exit(0);
}

racs_create_command(format) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg1;
    msgpack_unpacked_init(&msg1);

    msgpack_unpacked msg2;
    msgpack_unpacked_init(&msg2);

    racs_parse_buf(in_buf, &pk, &msg1, "Error parsing args")
    racs_parse_buf(out_buf, &pk, &msg2, "Error parsing buffer")

    racs_validate_num_args(&pk, msg1, 4)

    racs_validate_type(&pk, msg1, 0, MSGPACK_OBJECT_STR, "Invalid type at arg 1. Expected: string")
    racs_validate_type(&pk, msg1, 1, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 2. Expected: int")
    racs_validate_type(&pk, msg1, 2, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 3. Expected: int")
    racs_validate_type(&pk, msg1, 3, MSGPACK_OBJECT_POSITIVE_INTEGER, "Invalid type at arg 3. Expected: int")

    char *type = racs_unpack_str(&msg2.data, 0);
    if (strcmp(type, "i32v") != 0) {
        free(type);
        msgpack_sbuffer_clear(out_buf);
        return racs_pack_error(&pk, "Invalid input type. Expected: int32 array");
    }

    racs_int32 *in = racs_unpack_s32v(&msg2.data, 1);
    size_t size = racs_unpack_s32v_size(&msg2.data, 1);

    char *mime_type = racs_unpack_str(&msg1.data, 0);
    racs_uint32 sample_rate = racs_unpack_uint32(&msg1.data, 1);
    racs_uint16 channels = racs_unpack_uint16(&msg1.data, 2);
    racs_uint16 bit_depth = racs_unpack_uint16(&msg1.data, 3);

    void *out = malloc(size * 2 + 44);

    racs_format fmt;
    fmt.channels = channels;
    fmt.sample_rate = sample_rate;
    fmt.bit_depth = bit_depth;

    size_t n = racs_format_pcm(&fmt, in, out, size / channels, size * 2 + 44, mime_type);

    if (n != 0) {
        msgpack_sbuffer_clear(out_buf);
        int rc = racs_pack_u8v(&pk, out, n);

        free(type);
        free(mime_type);
        free(out);

        return rc;
    }

    free(type);
    free(mime_type);
    free(out);

    msgpack_sbuffer_clear(out_buf);
    return racs_pack_error(&pk, "Unsupported format");
}

int racs_stream(msgpack_sbuffer *out_buf, racs_context *ctx, racs_uint8 *data) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    int rc = racs_streamappend(ctx->mcache, ctx->mmt, ctx->kv, data);
    if (rc == RACS_STREAM_OK)
        return racs_pack_null_with_status_ok(&pk);

    return racs_pack_error(&pk, racs_stream_status_string[rc]);
}