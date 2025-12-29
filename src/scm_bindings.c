// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "scm_bindings.h"

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

SCM racs_scm_streamcreate(SCM stream_id, SCM sample_rate, SCM channels, SCM bit_depth) {
    char *cmd = NULL;
    asprintf(&cmd, "CREATE '%s' %d %d %d",
             scm_to_locale_string(stream_id),
             scm_to_uint32(sample_rate),
             scm_to_uint32(channels),
             scm_to_uint32(bit_depth));

    racs_db *db = racs_db_instance();
    racs_result res = racs_db_exec(db, cmd);

    free(cmd);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, (char *) res.data, res.size, 0) == MSGPACK_UNPACK_PARSE_ERROR) {
        free(res.data);
        scm_misc_error("create", "Deserialization error", SCM_EOL);
    }

    char *type = racs_unpack_str(&msg.data, 0);
    if (strcmp(type, "error") == 0) {
        racs_scm_propagate_error(&msg.data, res.data);
    }

    return SCM_EOL;
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

SCM racs_scm_streamlist(SCM pattern) {
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

SCM racs_scm_streamopen(SCM stream_id) {
    char *cmd = NULL;
    asprintf(&cmd, "OPEN '%s'", scm_to_locale_string(stream_id));

    racs_db *db = racs_db_instance();
    racs_result res = racs_db_exec(db, cmd);

    free(cmd);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, (char *) res.data, res.size, 0) == MSGPACK_UNPACK_PARSE_ERROR) {
        free(res.data);
        scm_misc_error("open", "Deserialization error", SCM_EOL);
    }

    char *type = racs_unpack_str(&msg.data, 0);
    if (strcmp(type, "error") == 0) {
        racs_scm_propagate_error(&msg.data, res.data);
    }

    return SCM_EOL;
}

SCM racs_scm_streamclose(SCM stream_id) {
    char *cmd = NULL;
    asprintf(&cmd, "CLOSE '%s'", scm_to_locale_string(stream_id));

    racs_db *db = racs_db_instance();
    racs_result res = racs_db_exec(db, cmd);

    free(cmd);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, (char *) res.data, res.size, 0) == MSGPACK_UNPACK_PARSE_ERROR) {
        free(res.data);
        scm_misc_error("close", "Deserialization error", SCM_EOL);
    }

    char *type = racs_unpack_str(&msg.data, 0);
    if (strcmp(type, "error") == 0) {
        racs_scm_propagate_error(&msg.data, res.data);
    }

    return SCM_EOL;
}

SCM racs_scm_shutdown() {
    racs_db *db = racs_db_instance();
    racs_db_exec(db, "SHUTDOWN");

    return SCM_EOL;
}

SCM racs_scm_ping() {
    return scm_from_locale_string("PONG");
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
    scm_c_define_gsubr("list", 1, 0, 0, racs_scm_streamlist);
    scm_c_define_gsubr("ping", 0, 0, 0, racs_scm_ping);

    scm_c_export("range", "meta", "encode", "list", "ping", NULL);
}

void racs_scm_init_module() {
    scm_c_define_module("racs", racs_scm_init_bindings, NULL);
}
