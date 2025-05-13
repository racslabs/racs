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

    char *type = racs_deserialize_str(&msg.data, 0);
    if (strcmp(type, "error") == 0) {
        racs_scm_propagate_error(&msg.data, res.data);
    }

    if (strcmp(type, "null") == 0) {
        free(res.data);
        return SCM_EOL;
    }

    size_t size = racs_deserialize_s32v_size(&msg.data, 1);
    racs_int32 *data = racs_deserialize_s32v(&msg.data, 1);

    return scm_take_s32vector(data, size);
}

SCM racs_scm_streamcreate(SCM stream_id, SCM sample_rate, SCM channels) {
    char *cmd = NULL;
    asprintf(&cmd, "STREAMCREATE '%s' %d %d",
             scm_to_locale_string(stream_id),
             scm_to_uint32(sample_rate),
             scm_to_uint32(channels));

    racs_db *db = racs_db_instance();
    racs_result res = racs_db_exec(db, cmd);

    free(cmd);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, (char *) res.data, res.size, 0) == MSGPACK_UNPACK_PARSE_ERROR) {
        free(res.data);
        scm_misc_error("streamcreate", "Deserialization error", SCM_EOL);
    }

    char *type = racs_deserialize_str(&msg.data, 0);
    if (strcmp(type, "error") == 0) {
        racs_scm_propagate_error(&msg.data, res.data);
    }

    return SCM_EOL;
}

SCM racs_scm_streaminfo(SCM stream_id, SCM attr) {
    char *cmd = NULL;
    asprintf(&cmd, "STREAMINFO '%s' '%s'",
             scm_to_locale_string(stream_id),
             scm_to_locale_string(attr));

    racs_db *db = racs_db_instance();
    racs_result res = racs_db_exec(db, cmd);

    free(cmd);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, (char *) res.data, res.size, 0) == MSGPACK_UNPACK_PARSE_ERROR) {
        free(res.data);
        scm_misc_error("streaminfo", "Deserialization error", SCM_EOL);
    }

    char *type = racs_deserialize_str(&msg.data, 0);
    if (strcmp(type, "error") == 0) {
        racs_scm_propagate_error(&msg.data, res.data);
    }

    if (strcmp(type, "null") == 0) {
        free(res.data);
        return SCM_EOL;
    }

    racs_uint64 value = racs_deserialize_uint64(&msg.data, 1);
    return scm_from_uint64(value);
}

void racs_scm_init_bindings() {
    scm_c_define_gsubr("extract", 3, 0, 0, racs_scm_extract);
    scm_c_define_gsubr("streamcreate", 4, 0, 0, racs_scm_streamcreate);
    scm_c_define_gsubr("streaminfo", 2, 0, 0, racs_scm_streaminfo);
}
