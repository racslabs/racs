#include "scm_bindings.h"

SCM racs_scm_extract(SCM stream_id, SCM from, SCM to) {
    char *cmd = NULL;
    asprintf(&cmd, "EXTRACT '%s' %s %s",
             scm_to_locale_string(stream_id),
             scm_to_locale_string(from),
             scm_to_locale_string(to));

    racs_db *db = racs_db_instance();
    racs_result res = racs_db_exec(db, cmd);

    free(cmd);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, (char *) res.data, res.size, 0) == MSGPACK_UNPACK_PARSE_ERROR) {
        free(res.data);
        scm_misc_error("extract", "Deserialization error", SCM_EOL);
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

SCM racs_scm_streaminfo(SCM stream_id, SCM attr) {
    char *cmd = NULL;
    asprintf(&cmd, "INFO '%s' '%s'",
             scm_to_locale_string(stream_id),
             scm_to_locale_string(attr));

    racs_db *db = racs_db_instance();
    racs_result res = racs_db_exec(db, cmd);

    free(cmd);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, (char *) res.data, res.size, 0) == MSGPACK_UNPACK_PARSE_ERROR) {
        free(res.data);
        scm_misc_error("info", "Deserialization error", SCM_EOL);
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

SCM racs_scm_format(SCM data, SCM mime_type, SCM sample_rate, SCM channels, SCM bit_depth) {
    char *_mime_type = scm_to_locale_string(mime_type);
    racs_uint32 _sample_rate = scm_to_uint32(sample_rate);
    racs_uint16 _channels = scm_to_uint16(channels);
    racs_uint16 _bit_depth = scm_to_uint16(bit_depth);

    scm_t_array_handle handle;
    scm_array_get_handle(data, &handle);

    const racs_int32 *in = scm_array_handle_s32_elements(&handle);
    size_t size = scm_c_array_length(data);

    void *out = malloc(size * (_bit_depth / 8) + 44);

    racs_format fmt;
    fmt.channels = _channels;
    fmt.sample_rate = _sample_rate;
    fmt.bit_depth = _bit_depth;

    size_t n = racs_format_pcm(&fmt, in, out, size / _channels, size * (_bit_depth / 8) + 44, _mime_type);

    scm_array_handle_release(&handle);
    return scm_take_u8vector((uint8_t *)out, n);
}

void racs_scm_init_bindings() {
    scm_c_define_gsubr("extract", 3, 0, 0, racs_scm_extract);
    scm_c_define_gsubr("create", 4, 0, 0, racs_scm_streamcreate);
    scm_c_define_gsubr("info", 2, 0, 0, racs_scm_streaminfo);
    scm_c_define_gsubr("format", 5, 0, 0, racs_scm_format);
}
