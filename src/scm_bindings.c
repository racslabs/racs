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

    size_t out_size;
    racs_int32 *out = racs_daw_ops_mix(_in_a, _in_a_len, _in_b, _in_b_len, &out_size);

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

    racs_int32 *out = racs_daw_ops_gain(_in, _in_len, _gain);
    scm_array_handle_release(&handle);

    return scm_take_s32vector(out, _in_len);
}

SCM racs_scm_range(SCM stream_id, SCM from, SCM to) {
    char *cmd = NULL;
    asprintf(&cmd, "RANGE '%s' %f %f",
             scm_to_locale_string(stream_id),
             scm_to_double(from),
             scm_to_double(to));

    racs_db *db = racs_db_instance();
    racs_result res = racs_db_exec(db, cmd);

    free(cmd);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, (char *) res.data, res.size, 0) == MSGPACK_UNPACK_PARSE_ERROR) {
        free(res.data);
        scm_misc_error("range", "Deserialization error", SCM_EOL);
    }

    char *type = racs_unpack_str(&msg.data, 0);
    if (strcmp(type, "error") == 0) {
        racs_scm_propagate_error(&msg.data, res.data);
    }

    if (strcmp(type, "null") == 0) {
        free(res.data);
        return SCM_EOL;
    }

    size_t size = racs_unpack_s32v_size(&msg.data, 1);
    racs_int32 *data = racs_unpack_s32v(&msg.data, 1);

    return scm_take_s32vector(data, size);
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

SCM racs_scm_encode(SCM data, SCM mime_type, SCM sample_rate, SCM channels, SCM bit_depth) {
    char *_mime_type = scm_to_locale_string(mime_type);
    racs_uint32 _sample_rate = scm_to_uint32(sample_rate);
    racs_uint16 _channels = scm_to_uint16(channels);
    racs_uint16 _bit_depth = scm_to_uint16(bit_depth);

    scm_t_array_handle handle;
    scm_array_get_handle(data, &handle);

    const racs_int32 *in = scm_array_handle_s32_elements(&handle);
    size_t size = scm_c_array_length(data);

    void *out = malloc(size * (_bit_depth / 8) + 44);

    racs_encode fmt;
    fmt.channels = _channels;
    fmt.sample_rate = _sample_rate;
    fmt.bit_depth = _bit_depth;

    size_t n = racs_encode_pcm(&fmt, in, out, size / _channels, size * (_bit_depth / 8) + 44, _mime_type);

    scm_array_handle_release(&handle);
    return scm_take_u8vector((uint8_t *)out, n);
}

void racs_scm_init_bindings() {
    scm_c_define_gsubr("range", 3, 0, 0, racs_scm_range);
    scm_c_define_gsubr("meta", 2, 0, 0, racs_scm_metadata);
    scm_c_define_gsubr("encode", 5, 0, 0, racs_scm_encode);
    scm_c_define_gsubr("list", 1, 0, 0, racs_scm_stream_list);
    scm_c_define_gsubr("mix", 2, 0, 0, racs_scm_mix);
    scm_c_define_gsubr("gain", 2, 0, 0, racs_scm_gain);

    scm_c_export("range", "meta", "encode", "list", "mix", "gain", NULL);
}

void racs_scm_init_module() {
    scm_c_define_module("racs", racs_scm_init_bindings, NULL);
}
