#include "scm_bindings.h"

SCM auxts_scm_extract(SCM stream_id, SCM from, SCM to) {
    char* cmd = NULL;
    asprintf(&cmd, "EXTRACT '%s' %s %s",
             scm_to_locale_string(stream_id),
             scm_to_locale_string(from),
             scm_to_locale_string(to));

    auxts_db* db = auxts_db_instance();
    auxts_result res = auxts_db_execute(db, cmd);

    free(cmd);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, (char*)res.data, res.size, 0) == MSGPACK_UNPACK_PARSE_ERROR) {
        free(res.data);
        scm_misc_error("extract", "Deserialization error", SCM_EOL);
    }

    char* type = auxts_deserialize_str(&msg.data, 0);
    if (strcmp(type, "error") == 0) {
        auxts_scm_propagate_error(&msg.data, res.data);
    }

    if (strcmp(type, "none") == 0) {
        free(res.data);
        return SCM_EOL;
    }

    size_t size = auxts_deserialize_i32v_size(&msg.data, 1);
    int32_t* data = auxts_deserialize_i32v(&msg.data, 1);

    return scm_take_s32vector(data, size);
}

SCM auxts_scm_create(SCM stream_id, SCM sample_rate, SCM channels, SCM bit_depth) {
    char* cmd = NULL;
    asprintf(&cmd, "CREATE '%s' %d %d %d",
             scm_to_locale_string(stream_id),
             scm_to_uint32(sample_rate),
             scm_to_uint32(channels),
             scm_to_uint32(bit_depth));

    auxts_db* db = auxts_db_instance();
    auxts_result res = auxts_db_execute(db, cmd);

    free(cmd);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);

    if (msgpack_unpack_next(&msg, (char*)res.data, res.size, 0) == MSGPACK_UNPACK_PARSE_ERROR) {
        free(res.data);
        scm_misc_error("extract", "Deserialization error", SCM_EOL);
    }

    char* type = auxts_deserialize_str(&msg.data, 0);
    if (strcmp(type, "error") == 0) {
        auxts_scm_propagate_error(&msg.data, res.data);
    }

    return SCM_EOL;
}

void auxts_scm_init_bindings() {
    scm_c_define_gsubr("extract", 3, 0, 0, auxts_scm_extract);
    scm_c_define_gsubr("create", 4, 0, 0, auxts_scm_create);
}
