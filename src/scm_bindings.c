// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "scm_bindings.h"

SCM racs_scm_mix(SCM in_a, SCM in_b) {
    scm_t_array_handle handle_a;
    scm_array_get_handle(in_a, &handle_a);

    const racs_int32 *_in_a = scm_array_handle_s32_elements(&handle_a);
    size_t _in_a_len = scm_c_array_length(in_a);

    scm_t_array_handle handle_b;
    scm_array_get_handle(in_b, &handle_b);

    const racs_int32 *_in_b = scm_array_handle_s32_elements(&handle_b);
    size_t _in_b_len = scm_c_array_length(in_b);

    if ((ssize_t) _in_a_len < 2 || (ssize_t) _in_b_len < 2) {
        scm_array_handle_release(&handle_a);
        scm_array_handle_release(&handle_b);
        scm_misc_error("trim", "Missing input data.", SCM_EOL);
    }

    size_t out_size;
    racs_int32 *out = racs_ops_mix(_in_a, _in_a_len, _in_b, _in_b_len, &out_size);

    scm_array_handle_release(&handle_a);
    scm_array_handle_release(&handle_b);

    return scm_take_s32vector(out, out_size);
}

SCM racs_scm_gain(SCM in, SCM gain) {
    double _gain = scm_to_double(gain);

    scm_t_array_handle handle;
    scm_array_get_handle(in, &handle);

    const racs_int32 *_in = scm_array_handle_s32_elements(&handle);
    size_t _in_len = scm_c_array_length(in);

    if ((ssize_t) _in_len < 2) {
        scm_array_handle_release(&handle);
        scm_misc_error("gain", "Missing input data.", SCM_EOL);
    }

    racs_int32 *out = racs_ops_gain(_in, _in_len, _gain);
    scm_array_handle_release(&handle);

    return scm_take_s32vector(out, _in_len);
}

SCM racs_scm_trim(SCM in, SCM left_seconds, SCM right_seconds) {
    double _left_seconds = scm_to_double(left_seconds);
    double _right_seconds = scm_to_double(right_seconds);

    scm_t_array_handle handle;
    scm_array_get_handle(in, &handle);

    const racs_int32 *_in = scm_array_handle_s32_elements(&handle);
    size_t _in_len = scm_c_array_length(in);

    if ((ssize_t) _in_len < 2) {
        scm_array_handle_release(&handle);
        scm_misc_error("trim", "Missing input data.", SCM_EOL);
    }

    size_t out_size;
    racs_int32 *out = racs_ops_trim(_in, _in_len, _left_seconds, _right_seconds, &out_size);

    scm_array_handle_release(&handle);

    return scm_take_s32vector(out, out_size);
}

SCM racs_scm_fade(SCM in, SCM fade_in_seconds, SCM fade_out_seconds) {
    double _fade_in_seconds = scm_to_double(fade_in_seconds);
    double _fade_out_seconds = scm_to_double(fade_out_seconds);

    scm_t_array_handle handle;
    scm_array_get_handle(in, &handle);

    const racs_int32 *_in = scm_array_handle_s32_elements(&handle);
    size_t _in_len = scm_c_array_length(in);

    if ((ssize_t) _in_len < 2) {
        scm_array_handle_release(&handle);
        scm_misc_error("fade", "Missing input data.", SCM_EOL);
    }

    size_t out_size;
    racs_int32 *out = racs_ops_fade(_in, _in_len, _fade_in_seconds, _fade_out_seconds, &out_size);

    scm_array_handle_release(&handle);

    return scm_take_s32vector(out, out_size);
}

SCM racs_scm_pan(SCM in, SCM pan) {
    double _pan = scm_to_double(pan);

    if (_pan < -1.0 || _pan > 1.0)
        scm_misc_error("pan", "Pan must be between -1.0 and +1.0", SCM_EOL);

    scm_t_array_handle handle;
    scm_array_get_handle(in, &handle);

    const racs_int32 *_in = scm_array_handle_s32_elements(&handle);
    size_t _in_len = scm_c_array_length(in);

    if ((ssize_t) _in_len < 2) {
        scm_array_handle_release(&handle);
        scm_misc_error("pan", "Missing input data.", SCM_EOL);
    }

    racs_uint32 channels = (racs_uint16)(_in[1] >> 16);
    if (channels < 2) {
        scm_array_handle_release(&handle);
        scm_misc_error("pan", "Invalid number of channels.", SCM_EOL);
    }

    size_t out_size;
    racs_int32 *out = racs_ops_pan(_in, _in_len, _pan, &out_size);

    scm_array_handle_release(&handle);
    return scm_take_s32vector(out, out_size);
}

SCM racs_scm_pad(SCM in, SCM left_seconds, SCM right_seconds) {
    double _left_seconds = scm_to_double(left_seconds);
    double _right_seconds = scm_to_double(right_seconds);

    scm_t_array_handle handle;
    scm_array_get_handle(in, &handle);

    const racs_int32 *_in = scm_array_handle_s32_elements(&handle);
    size_t _in_len = scm_c_array_length(in);

    if ((ssize_t) _in_len < 2) {
        scm_array_handle_release(&handle);
        scm_misc_error("pad", "Missing input data.", SCM_EOL);
    }

    size_t out_size;
    racs_int32 *out = racs_ops_pad(_in, _in_len, _left_seconds, _right_seconds, &out_size);

    scm_array_handle_release(&handle);
    return scm_take_s32vector(out, out_size);
}

SCM racs_scm_clip(SCM in, SCM min, SCM max) {
    racs_int32 _min = scm_to_int32(min);
    racs_int32 _max = scm_to_int32(max);

    scm_t_array_handle handle;
    scm_array_get_handle(in, &handle);

    const racs_int32 *_in = scm_array_handle_s32_elements(&handle);
    size_t _in_len = scm_c_array_length(in);

    if ((ssize_t) _in_len < 2) {
        scm_array_handle_release(&handle);
        scm_misc_error("clip", "Missing input data.", SCM_EOL);
    }

    size_t out_size;
    racs_int32 *out = racs_ops_clip(_in, _in_len, _min, _max, &out_size);

    scm_array_handle_release(&handle);
    return scm_take_s32vector(out, out_size);
}

SCM racs_scm_split(SCM in, SCM channel) {
    racs_uint16 _channel = scm_to_uint16(channel);

    scm_t_array_handle handle;
    scm_array_get_handle(in, &handle);

    const racs_int32 *_in = scm_array_handle_s32_elements(&handle);
    size_t _in_len = scm_c_array_length(in);

    if ((ssize_t) _in_len < 2) {
        scm_array_handle_release(&handle);
        scm_misc_error("split", "Missing input data.", SCM_EOL);
    }

    racs_uint16 channels = (racs_uint16)(_in[1] >> 16);
    if (channels != 2) {
        scm_array_handle_release(&handle);
        scm_misc_error("split", "Invalid number of channels.", SCM_EOL);
    }

    if (_channel > 1) {
        scm_array_handle_release(&handle);
        scm_misc_error("split", "Invalid channel number.", SCM_EOL);
    }

    size_t out_size;
    racs_int32 *out = racs_ops_split(_in, _in_len, _channel, &out_size);

    scm_array_handle_release(&handle);
    return scm_take_s32vector(out, out_size);
}

SCM racs_scm_merge(SCM in_a, SCM in_b) {
    scm_t_array_handle handle_a;
    scm_array_get_handle(in_a, &handle_a);

    const racs_int32 *_in_a = scm_array_handle_s32_elements(&handle_a);
    size_t _in_a_len = scm_c_array_length(in_a);

    scm_t_array_handle handle_b;
    scm_array_get_handle(in_b, &handle_b);

    const racs_int32 *_in_b = scm_array_handle_s32_elements(&handle_b);
    size_t _in_b_len = scm_c_array_length(in_b);

    if ((ssize_t) _in_a_len < 2 || (ssize_t) _in_b_len < 2) {
        scm_array_handle_release(&handle_a);
        scm_array_handle_release(&handle_b);
        scm_misc_error("merge", "Missing input data.", SCM_EOL);
    }

    racs_uint16 channel_a = (racs_uint16)(_in_a[1] >> 16);
    racs_uint16 channel_b = (racs_uint16)(_in_b[1] >> 16);

    if (channel_a != 1 || channel_b != 1) {
        scm_array_handle_release(&handle_a);
        scm_array_handle_release(&handle_b);
        scm_misc_error("merge", "Inputs must be mono.", SCM_EOL);
    }

    if (_in_a[0] != _in_b[0]) {
        scm_array_handle_release(&handle_a);
        scm_array_handle_release(&handle_b);
        scm_misc_error("merge", "Inputs must have the same sample rate.", SCM_EOL);
    }

    if ((_in_a[1] & 0x0000ffff) != (_in_b[1] & 0x0000ffff)) {
        scm_array_handle_release(&handle_a);
        scm_array_handle_release(&handle_b);
        scm_misc_error("merge", "Inputs must have the same bit depth.", SCM_EOL);
    }

    size_t out_size;
    racs_int32 *out = racs_ops_merge(_in_a, _in_a_len, _in_b, _in_b_len, &out_size);

    scm_array_handle_release(&handle_a);
    scm_array_handle_release(&handle_b);

    return scm_take_s32vector(out, out_size);
}

SCM racs_scm_range(SCM stream_id, SCM start, SCM duration) {
    char *_stream_id = scm_to_locale_string(stream_id);
    double _start = scm_to_double(start);
    double _duration = scm_to_double(duration);

    racs_pcm pcm;

    racs_db *db = racs_db_instance();

    int rc = racs_range(&db->ctx, &pcm, _stream_id, _start, _duration);
    if (rc == RACS_RANGE_STATUS_NOT_FOUND) {
        racs_pcm_destroy(&pcm);
        scm_misc_error("range", "The stream-id does not exist", SCM_EOL);
    }

    racs_int32 *samples = NULL;
    size_t size = pcm.samples * pcm.channels;

    if (pcm.bit_depth == 16)
        samples = racs_s16_s32((const racs_int16 *) pcm.out_stream.data, size);
    if (pcm.bit_depth == 24)
        samples = racs_s24_s32((const racs_int24 *) pcm.out_stream.data, size);
    free(pcm.out_stream.data);

    if (!samples)
        scm_misc_error("range", "Unknown error", SCM_EOL);

    // pre-pend sample-rate, channels and bit-depth
    samples[0] = (racs_int32)pcm.sample_rate;
    samples[1] = (uint16_t)pcm.channels << 16 | (uint16_t)pcm.bit_depth;

    return scm_take_s32vector(samples, size + 2);
}

SCM racs_scm_metadata(SCM stream_id, SCM attr) {
    char *cmd = NULL;
    asprintf(&cmd, "META '%s' '%s'",
             scm_to_locale_string(stream_id),
             scm_to_locale_string(attr));

    racs_db *db = racs_db_instance();
    racs_result res = racs_db_exec(db, cmd);

    free(cmd);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, (char *) res.data, res.size, 0) == MSGPACK_UNPACK_PARSE_ERROR) {
        free(res.data);
        scm_misc_error("meta", "Deserialization error", SCM_EOL);
    }

    char *type = racs_unpack_str(&msg.data, 0);
    if (strcmp(type, "error") == 0) {
        racs_scm_propagate_error(&msg.data, res.data);
    }

    if (strcmp(type, "null") == 0) {
        free(res.data);
        return SCM_EOL;
    }

    racs_int64 value = racs_unpack_int64(&msg.data, 1);
    return scm_from_int64(value);
}

SCM racs_scm_stream_list(SCM pattern) {
    char *cmd = NULL;
    asprintf(&cmd, "LIST '%s'", scm_to_locale_string(pattern));

    racs_db *db = racs_db_instance();
    racs_result res = racs_db_exec(db, cmd);

    free(cmd);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, (char *) res.data, res.size, 0) == MSGPACK_UNPACK_PARSE_ERROR) {
        free(res.data);
        scm_misc_error("list", "Deserialization error", SCM_EOL);
    }

    char *type = racs_unpack_str(&msg.data, 0);
    if (strcmp(type, "error") == 0) {
        racs_scm_propagate_error(&msg.data, res.data);
    }

    SCM list = SCM_EOL;
    size_t size = msg.data.via.array.size;

    for (int i = 1; i < size; i++) {
        char *str = racs_unpack_str(&msg.data, i);
        SCM _str = scm_from_locale_string(str);
        list = scm_cons(_str, list);
        free(str);
    }

    return list;
}

SCM racs_scm_encode(SCM data, SCM mime_type) {
    char *_mime_type = scm_to_locale_string(mime_type);

    scm_t_array_handle handle;
    scm_array_get_handle(data, &handle);

    const racs_int32 *in = scm_array_handle_s32_elements(&handle);
    size_t size = scm_c_array_length(data) - 2;

    if ((ssize_t) size < 2) {
        scm_array_handle_release(&handle);
        scm_misc_error("encode", "Missing input data.", SCM_EOL);
    }

    racs_encode encode;

    // get pre-pended sample-rate, channels and bit-depth
    encode.sample_rate = in[0];
    encode.channels    = (racs_uint16)(in[1] >> 16);
    encode.bit_depth   = (racs_uint16)(in[1] & 0xffff);

    void *out = malloc(size * (encode.bit_depth / 8) + 44);
    size_t n = racs_encode_pcm(&encode, in + 2, out, size / encode.channels, size * (encode.bit_depth / 8) + 44, _mime_type);

    scm_array_handle_release(&handle);
    return scm_take_u8vector(out, n);
}

void racs_scm_init_bindings() {
    scm_c_define_gsubr("range", 3, 0, 0, racs_scm_range);
    scm_c_define_gsubr("meta", 2, 0, 0, racs_scm_metadata);
    scm_c_define_gsubr("encode", 2, 0, 0, racs_scm_encode);
    scm_c_define_gsubr("list", 1, 0, 0, racs_scm_stream_list);
    scm_c_define_gsubr("mix", 2, 0, 0, racs_scm_mix);
    scm_c_define_gsubr("gain", 2, 0, 0, racs_scm_gain);
    scm_c_define_gsubr("trim", 3, 0, 0, racs_scm_trim);
    scm_c_define_gsubr("fade", 3, 0, 0, racs_scm_fade);
    scm_c_define_gsubr("pan", 2, 0, 0, racs_scm_pan);
    scm_c_define_gsubr("pad", 3, 0, 0, racs_scm_pad);
    scm_c_define_gsubr("clip", 3, 0, 0, racs_scm_clip);
    scm_c_define_gsubr("split", 2, 0, 0, racs_scm_split);
    scm_c_define_gsubr("merge", 2, 0, 0, racs_scm_merge);

    scm_c_export("range", "meta", "encode", "list", "mix", "gain", "trim", "fade", "pan", "pad", "clip", "split", "merge", NULL);
}

void racs_scm_init_module() {
    scm_c_define_module("racs", racs_scm_init_bindings, NULL);
}
