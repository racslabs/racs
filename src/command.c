// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "command.h"

racs_create_command(ping) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "PING", "Error parsing args")
    racs_validate_num_args(&pk, msg, "PING", 0)

    return racs_pack_str(&pk, "PONG");
}

racs_create_command(streamcreate) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "CREATE", "Error parsing args")

    racs_validate_num_args(&pk, msg, "CREATE", 4)
    racs_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "CREATE", "Invalid type at arg 1. Expected: string")
    racs_validate_arg_type(&pk, msg, 1, MSGPACK_OBJECT_POSITIVE_INTEGER, "CREATE", "Invalid type at arg 2. Expected: positive int")
    racs_validate_arg_type(&pk, msg, 2, MSGPACK_OBJECT_POSITIVE_INTEGER, "CREATE", "Invalid type at arg 3. Expected: positive int")
    racs_validate_arg_type(&pk, msg, 3, MSGPACK_OBJECT_POSITIVE_INTEGER, "CREATE", "Invalid type at arg 4. Expected: positive int")

    char *stream_id = racs_unpack_str(&msg.data, 0);

    racs_uint32 sample_rate = racs_unpack_uint32(&msg.data, 1);
    racs_uint16 channels = racs_unpack_uint16(&msg.data, 2);
    racs_uint16 bit_depth = racs_unpack_uint16(&msg.data, 3);

    if (bit_depth != 16 && bit_depth != 24)
        return racs_pack_error(&pk, "CREATE", "Invalid bit-depth. Only 16- and 24-bit are supported.");

    if (channels != 1 && channels != 2)
        return racs_pack_error(&pk, "CREATE", "Invalid number of channels. Must be mono or stereo.");

    int rc = racs_stream_create(stream_id, sample_rate, channels, bit_depth);
    if (rc == 1)
        return racs_pack_null_with_status_ok(&pk);

    return racs_pack_error(&pk, "CREATE", "Stream already exist.");
}

racs_create_command(streamlist) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "LIST", "Error parsing args")
    racs_validate_num_args(&pk, msg, "LIST", 1)
    racs_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "LIST", "Invalid type at arg 1. Expected string")

    char *pattern = racs_unpack_str(&msg.data, 0);

    racs_streams streams;
    racs_streams_init(&streams);
    racs_streams_list(&streams, pattern);

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

    racs_parse_buf(in_buf, &pk, &msg, "OPEN", "Error parsing args")

    racs_validate_num_args(&pk, msg, "OPEN", 1)
    racs_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "OPEN", "Invalid type at arg 1. Expected string")

    char *stream_id = racs_unpack_str(&msg.data, 0);
    int rc = racs_stream_open(ctx->kv, racs_hash(stream_id));
    free(stream_id);

    if (rc == 1)
        return racs_pack_null_with_status_ok(&pk);

    return racs_pack_error(&pk, "OPEN", "Stream is already open");
}

racs_create_command(streamclose) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "CLOSE", "Error parsing args")

    racs_validate_num_args(&pk, msg, "CLOSE", 1)
    racs_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "CLOSE", "Invalid type at arg 1. Expected string")

    char *stream_id = racs_unpack_str(&msg.data, 0);
    int rc = racs_stream_close(ctx->kv, racs_hash(stream_id));
    free(stream_id);

    if (rc == 1)
        return racs_pack_null_with_status_ok(&pk);

    return racs_pack_error(&pk, "CLOSE", "Stream is not open");
}

racs_create_command(metadata) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "META", "Error parsing args")

    racs_validate_num_args(&pk, msg, "META", 2)
    racs_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "META", "Invalid type at arg 1. Expected string")
    racs_validate_arg_type(&pk, msg, 1, MSGPACK_OBJECT_STR, "META", "Invalid type at arg 2. Expected string")

    char *stream_id = racs_unpack_str(&msg.data, 0);
    char *attr = racs_unpack_str(&msg.data, 1);

    racs_uint64 hash = racs_hash(stream_id);
    racs_int64 value = racs_metadata_attr(hash, attr);

    free(stream_id);
    free(attr);

    if (value == -1)
        return racs_pack_error(&pk, "META", "The stream-id does not exist.");

    if (value == 0)
        return racs_pack_error(&pk, "META", "Invalid metadata attribute.");

    return racs_pack_int64(&pk, value);
}

racs_create_command(eval) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "EVAL", "Error parsing args")

    racs_validate_num_args(&pk, msg, "EVAL", 1)
    racs_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "EVAL", "Invalid type at arg 1. Expected string")

    char *error = NULL;
    char *expr = racs_unpack_str(&msg.data, 0);

    SCM res = racs_scm_safe_eval_with_error_handling(expr, &error);
    free(expr);

    if (error) return racs_pack_error(&pk, "EVAL", error);

    return racs_scm_pack(&pk, out_buf, res, is_final);
}

racs_create_command(range) {
    msgpack_sbuffer_clear(out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    racs_parse_buf(in_buf, &pk, &msg, "RANGE", "Error parsing args")

    racs_validate_num_args(&pk, msg, "RANGE", 3)
    racs_validate_arg_type(&pk, msg, 0, MSGPACK_OBJECT_STR, "RANGE", "Invalid type at arg 1. Expected: string")
    racs_validate_arg_type(&pk, msg, 1, MSGPACK_OBJECT_FLOAT64, "RANGE", "Invalid type at arg 2. Expected: float")
    racs_validate_arg_type(&pk, msg, 2, MSGPACK_OBJECT_FLOAT64, "RANGE", "Invalid type at arg 3. Expected: float")

    char *stream_id = racs_unpack_str(&msg.data, 0);
    double start = racs_unpack_float64(&msg.data, 1);
    double duration = racs_unpack_float64(&msg.data, 2);

    racs_pcm pcm;

    int rc = racs_range(ctx, &pcm, stream_id, start, duration);
    if (rc == RACS_RANGE_STATUS_NOT_FOUND) {
        free(stream_id);
        racs_pcm_destroy(&pcm);
        return racs_pack_error(&pk, "RANGE", "The stream-id does not exist");
    }

    racs_int32 *samples = NULL;
    size_t size = pcm.samples * pcm.channels;

    if (pcm.bit_depth == 16)
        samples = racs_s16_s32((const racs_int16 *) pcm.out_stream.data, size);
    if (pcm.bit_depth == 24)
        samples = racs_s24_s32((const racs_int24 *) pcm.out_stream.data, size);
    free(pcm.out_stream.data);

    if (!samples) {
        free(stream_id);
        return racs_pack_error(&pk, "RANGE", "Unknown error");
    }

    // pre-pend sample-rate, channels and bit-depth
    samples[0] = (racs_int32)pcm.sample_rate;
    samples[1] = (uint16_t)pcm.channels << 16 | (uint16_t)pcm.bit_depth;

    if (is_final)
        rc = racs_pack_s32v(&pk, samples + 2, size);
    else
        rc = racs_pack_s32v(&pk, samples, size + 2);

    free(samples);
    return rc;
}

racs_create_command(encode) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg1;
    msgpack_unpacked_init(&msg1);

    msgpack_unpacked msg2;
    msgpack_unpacked_init(&msg2);

    racs_parse_buf(in_buf, &pk, &msg1, "ENCODE", "Error parsing args")
    racs_parse_buf(out_buf, &pk, &msg2, "ENCODE", "Error parsing buffer")

    racs_validate_num_args(&pk, msg1, "ENCODE", 1)
    racs_validate_arg_type(&pk, msg1, 0, MSGPACK_OBJECT_STR, "ENCODE", "Invalid type at arg 1. Expected: string")

    racs_validate_not_null(&pk, msg2, "ENCODE")
    racs_validate_s32v(&pk, &msg2, "ENCODE")

    racs_int32 *in = racs_unpack_s32v(&msg2.data, 1);
    size_t size = racs_unpack_s32v_size(&msg2.data, 1) - 2;
    char *mime_type = racs_unpack_str(&msg1.data, 0);

    if (size == 0 || !in) {
        free(mime_type);
        msgpack_sbuffer_clear(out_buf);
        return racs_pack_error(&pk, "ENCODE", "Missing input data.");
    }

    racs_encode encode;

    // get pre-pended sample-rate, channels and bit-depth
    encode.sample_rate = in[0];
    encode.channels    = (racs_uint16)(in[1] >> 16);
    encode.bit_depth   = (racs_uint16)(in[1] & 0xffff);

    void *out = malloc(size * (encode.bit_depth / 8) + 44);

    size_t n = racs_encode_pcm(&encode, in + 2, out, size / encode.channels, size * (encode.bit_depth / 8) + 44, mime_type);
    if (n != 0) {
        msgpack_sbuffer_clear(out_buf);
        int rc = racs_pack_u8v(&pk, out, n);

        free(mime_type);
        free(out);

        return rc;
    }

    free(mime_type);
    free(out);

    msgpack_sbuffer_clear(out_buf);
    return racs_pack_error(&pk, "ENCODE", "Unsupported format");
}

racs_create_command(shutdown) {
    racs_context_destroy(ctx);
    exit(0);
}

racs_create_command(gain) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg1;
    msgpack_unpacked_init(&msg1);

    msgpack_unpacked msg2;
    msgpack_unpacked_init(&msg2);

    racs_parse_buf(in_buf, &pk, &msg1, "GAIN", "Error parsing args")
    racs_parse_buf(out_buf, &pk, &msg2, "GAIN", "Error parsing buffer")

    racs_validate_num_args(&pk, msg1, "GAIN", 1)
    racs_validate_arg_type(&pk, msg1, 0, MSGPACK_OBJECT_FLOAT64, "GAIN", "Invalid type at arg 1. Expected: float")

    racs_validate_not_null(&pk, msg2, "GAIN")
    racs_validate_s32v(&pk, &msg2, "GAIN")

    double gain = racs_unpack_float64(&msg1.data, 0);
    racs_int32 *in = racs_unpack_s32v(&msg2.data, 1);
    size_t in_size = racs_unpack_s32v_size(&msg2.data, 1);

    if (in_size < 2 || !in) {
        msgpack_sbuffer_clear(out_buf);
        return racs_pack_error(&pk, "GAIN", "Missing input data.");
    }

    racs_int32 *out = racs_ops_gain(in, in_size, gain);
    msgpack_sbuffer_clear(out_buf);

    int rc;
    if (is_final)
        rc = racs_pack_s32v(&pk, out + 2, in_size - 2);
    else
        rc = racs_pack_s32v(&pk, out, in_size);

    free(out);
    return rc;
}

racs_create_command(trim) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg1;
    msgpack_unpacked_init(&msg1);

    msgpack_unpacked msg2;
    msgpack_unpacked_init(&msg2);

    racs_parse_buf(in_buf, &pk, &msg1, "TRIM", "Error parsing args")
    racs_parse_buf(out_buf, &pk, &msg2, "TRIM", "Error parsing buffer")

    racs_validate_num_args(&pk, msg1, "TRIM", 2)
    racs_validate_arg_type(&pk, msg1, 0, MSGPACK_OBJECT_FLOAT64, "TRIM", "Invalid type at arg 1. Expected: float")
    racs_validate_arg_type(&pk, msg1, 1, MSGPACK_OBJECT_FLOAT64, "TRIM", "Invalid type at arg 1. Expected: float")

    racs_validate_not_null(&pk, msg2, "TRIM")
    racs_validate_s32v(&pk, &msg2, "TRIM")

    double left_seconds = racs_unpack_float64(&msg1.data, 0);
    double right_seconds = racs_unpack_float64(&msg1.data, 1);

    racs_int32 *in = racs_unpack_s32v(&msg2.data, 1);
    size_t in_size = racs_unpack_s32v_size(&msg2.data, 1);

    if (in_size < 2 || !in) {
        msgpack_sbuffer_clear(out_buf);
        return racs_pack_error(&pk, "TRIM", "Missing input data.");
    }

    size_t out_size;
    racs_int32 *out = racs_ops_trim(in, in_size, left_seconds, right_seconds, &out_size);

    msgpack_sbuffer_clear(out_buf);

    int rc;
    if (is_final)
        rc = racs_pack_s32v(&pk, out + 2, out_size - 2);
    else
        rc = racs_pack_s32v(&pk, out, out_size);

    free(out);
    return rc;
}

racs_create_command(fade) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg1;
    msgpack_unpacked_init(&msg1);

    msgpack_unpacked msg2;
    msgpack_unpacked_init(&msg2);

    racs_parse_buf(in_buf, &pk, &msg1, "FADE", "Error parsing args")
    racs_parse_buf(out_buf, &pk, &msg2, "FADE", "Error parsing buffer")

    racs_validate_num_args(&pk, msg1, "FADE", 2)
    racs_validate_arg_type(&pk, msg1, 0, MSGPACK_OBJECT_FLOAT64, "FADE", "Invalid type at arg 1. Expected: float")
    racs_validate_arg_type(&pk, msg1, 1, MSGPACK_OBJECT_FLOAT64, "FADE", "Invalid type at arg 1. Expected: float")

    racs_validate_not_null(&pk, msg2, "FADE")
    racs_validate_s32v(&pk, &msg2, "FADE")

    double fade_in_seconds = racs_unpack_float64(&msg1.data, 0);
    double fade_out_seconds = racs_unpack_float64(&msg1.data, 1);

    racs_int32 *in = racs_unpack_s32v(&msg2.data, 1);
    size_t in_size = racs_unpack_s32v_size(&msg2.data, 1);

    if (in_size < 2 || !in) {
        msgpack_sbuffer_clear(out_buf);
        return racs_pack_error(&pk, "FADE", "Missing input data.");
    }

    size_t out_size;
    racs_int32 *out = racs_ops_fade(in, in_size, fade_in_seconds, fade_out_seconds, &out_size);

    msgpack_sbuffer_clear(out_buf);

    int rc;
    if (is_final)
        rc = racs_pack_s32v(&pk, out + 2, out_size - 2);
    else
        rc = racs_pack_s32v(&pk, out, out_size);

    free(out);
    return rc;
}

racs_create_command(pan) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg1;
    msgpack_unpacked_init(&msg1);

    msgpack_unpacked msg2;
    msgpack_unpacked_init(&msg2);

    racs_parse_buf(in_buf, &pk, &msg1, "PAN", "Error parsing args")
    racs_parse_buf(out_buf, &pk, &msg2, "PAN", "Error parsing buffer")

    racs_validate_num_args(&pk, msg1, "PAN", 1)
    racs_validate_arg_type(&pk, msg1, 0, MSGPACK_OBJECT_FLOAT64, "PAN", "Invalid type at arg 1. Expected: float")

    racs_validate_not_null(&pk, msg2, "PAN")
    racs_validate_s32v(&pk, &msg2, "PAN")

    double pan = racs_unpack_float64(&msg1.data, 0);
    if (pan < -1.0 || pan > 1.0) {
        msgpack_sbuffer_clear(out_buf);
        return racs_pack_error(&pk, "PAN", "Pan must be between -1.0 and +1.0.");
    }

    racs_int32 *in = racs_unpack_s32v(&msg2.data, 1);
    size_t in_size = racs_unpack_s32v_size(&msg2.data, 1);

    if (in_size < 2 || !in) {
        msgpack_sbuffer_clear(out_buf);
        return racs_pack_error(&pk, "PAN", "Missing input data.");
    }

    size_t out_size;
    racs_int32 *out = racs_ops_pan(in, in_size, pan, &out_size);

    msgpack_sbuffer_clear(out_buf);

    int rc;
    if (is_final)
        rc = racs_pack_s32v(&pk, out + 2, out_size - 2);
    else
        rc = racs_pack_s32v(&pk, out, out_size);

    free(out);
    return rc;
}

racs_create_command(pad) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg1;
    msgpack_unpacked_init(&msg1);

    msgpack_unpacked msg2;
    msgpack_unpacked_init(&msg2);

    racs_parse_buf(in_buf, &pk, &msg1, "PAD", "Error parsing args")
    racs_parse_buf(out_buf, &pk, &msg2, "PAD", "Error parsing buffer")

    racs_validate_num_args(&pk, msg1, "PAD", 2)
    racs_validate_arg_type(&pk, msg1, 0, MSGPACK_OBJECT_FLOAT64, "PAD", "Invalid type at arg 1. Expected: float")
    racs_validate_arg_type(&pk, msg1, 1, MSGPACK_OBJECT_FLOAT64, "PAD", "Invalid type at arg 2. Expected: float")

    racs_validate_not_null(&pk, msg2, "PAD")
    racs_validate_s32v(&pk, &msg2, "PAD")

    double left_seconds = racs_unpack_float64(&msg1.data, 0);
    double right_seconds = racs_unpack_float64(&msg1.data, 1);

    racs_int32 *in = racs_unpack_s32v(&msg2.data, 1);
    size_t in_size = racs_unpack_s32v_size(&msg2.data, 1);

    if (in_size < 2 || !in) {
        msgpack_sbuffer_clear(out_buf);
        return racs_pack_error(&pk, "PAD", "Missing input data.");
    }

    size_t out_size;
    racs_int32 *out = racs_ops_pad(in, in_size, left_seconds, right_seconds, &out_size);

    msgpack_sbuffer_clear(out_buf);

    int rc;
    if (is_final)
        rc = racs_pack_s32v(&pk, out + 2, out_size - 2);
    else
        rc = racs_pack_s32v(&pk, out, out_size);

    free(out);
    return rc;
}

racs_create_command(clip) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg1;
    msgpack_unpacked_init(&msg1);

    msgpack_unpacked msg2;
    msgpack_unpacked_init(&msg2);

    racs_parse_buf(in_buf, &pk, &msg1, "CLIP", "Error parsing args")
    racs_parse_buf(out_buf, &pk, &msg2, "CLIP", "Error parsing buffer")

    racs_validate_num_args(&pk, msg1, "CLIP", 2)
    racs_validate_arg_type(&pk, msg1, 0, MSGPACK_OBJECT_NEGATIVE_INTEGER, "CLIP", "Invalid type at arg 1. Expected: negative int")
    racs_validate_arg_type(&pk, msg1, 1, MSGPACK_OBJECT_POSITIVE_INTEGER, "CLIP", "Invalid type at arg 2. Expected: positive int")

    racs_validate_not_null(&pk, msg2, "CLIP")
    racs_validate_s32v(&pk, &msg2, "CLIP")

    racs_int32 min = racs_unpack_int32(&msg1.data, 0);
    racs_int32 max = racs_unpack_int32(&msg1.data, 1);

    racs_int32 *in = racs_unpack_s32v(&msg2.data, 1);
    size_t in_size = racs_unpack_s32v_size(&msg2.data, 1);

    if (in_size < 2 || !in) {
        msgpack_sbuffer_clear(out_buf);
        return racs_pack_error(&pk, "CLIP", "Missing input data.");
    }

    size_t out_size;
    racs_int32 *out = racs_ops_clip(in, in_size, min, max, &out_size);

    msgpack_sbuffer_clear(out_buf);

    int rc;
    if (is_final)
        rc = racs_pack_s32v(&pk, out + 2, out_size - 2);
    else
        rc = racs_pack_s32v(&pk, out, out_size);

    free(out);
    return rc;
}

racs_create_command(split) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    msgpack_unpacked msg1;
    msgpack_unpacked_init(&msg1);

    msgpack_unpacked msg2;
    msgpack_unpacked_init(&msg2);

    racs_parse_buf(in_buf, &pk, &msg1, "SPLIT", "Error parsing args")
    racs_parse_buf(out_buf, &pk, &msg2, "SPLIT", "Error parsing buffer")

    racs_validate_num_args(&pk, msg1, "SPLIT", 1)
    racs_validate_arg_type(&pk, msg1, 0, MSGPACK_OBJECT_POSITIVE_INTEGER, "SPLIT", "Invalid type at arg 2. Expected: positive int")

    racs_validate_not_null(&pk, msg2, "SPLIT")
    racs_validate_s32v(&pk, &msg2, "SPLIT")

    racs_uint16 channel = racs_unpack_uint16(&msg1.data, 0);

    racs_int32 *in = racs_unpack_s32v(&msg2.data, 1);
    size_t in_size = racs_unpack_s32v_size(&msg2.data, 1);

    if (in_size < 2 || !in) {
        msgpack_sbuffer_clear(out_buf);
        return racs_pack_error(&pk, "SPLIT", "Missing input data.");
    }

    racs_uint16 channels = (racs_uint16)(in[1] >> 16);
    if (channels != 2) {
        msgpack_sbuffer_clear(out_buf);
        return racs_pack_error(&pk, "SPLIT", "Invalid number of channels.");
    }

    if (channel > 1) {
        msgpack_sbuffer_clear(out_buf);
        return racs_pack_error(&pk, "SPLIT", "Invalid channel number.");
    }

    size_t out_size;
    racs_int32 *out = racs_ops_split(in, in_size, channel, &out_size);

    msgpack_sbuffer_clear(out_buf);

    int rc;
    if (is_final)
        rc = racs_pack_s32v(&pk, out + 2, out_size - 2);
    else
        rc = racs_pack_s32v(&pk, out, out_size);

    free(out);
    return rc;
}

int racs_stream(msgpack_sbuffer *out_buf, racs_context *ctx, racs_uint8 *data) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    int rc = racs_stream_append(ctx->mmt, ctx->offsets, ctx->kv, data);
    if (rc == RACS_STREAM_OK)
        return racs_pack_null_with_status_ok(&pk);

    return racs_pack_error(&pk, "STREAM", racs_stream_status_string[rc]);
}

int racs_stream_batch(msgpack_sbuffer *out_buf, racs_context *ctx, racs_uint8 *data, size_t size) {
    msgpack_packer pk;
    msgpack_packer_init(&pk, out_buf, msgpack_sbuffer_write);

    racs_stream_batch_append(ctx->mmt, ctx->offsets, ctx->kv, data, size);
    return racs_pack_null_with_status_ok(&pk);
}
