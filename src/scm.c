#include "scm.h"

SCM auxts_scm_extract(SCM scm_stream_id, SCM scm_from, SCM scm_to) {
    const char* stream_id = scm_to_locale_string(scm_stream_id);
    const char *from = scm_to_locale_string(scm_from);
    const char *to = scm_to_locale_string(scm_to);

    char* cmd = NULL;
    asprintf(&cmd, "EXTRACT '%s' '%s' '%s'", stream_id, from, to);

    SCM bytevector = auxts_scm_execute(cmd);
    free(cmd);

    return bytevector;
}

SCM auxts_scm_create(SCM scm_stream_id, SCM scm_sample_rate, SCM scm_channels, SCM scm_bit_depth) {
    const char* stream_id = scm_to_locale_string(scm_stream_id);
    const uint32_t sample_rate = scm_to_uint32(scm_sample_rate);
    const uint32_t channels = scm_to_uint32(scm_channels);
    const uint32_t bit_depth = scm_to_uint32(scm_bit_depth);

    char* cmd = NULL;
    asprintf(&cmd, "CREATE '%s' %d %d %d", stream_id, sample_rate, channels, bit_depth);
    SCM bytevector = auxts_scm_execute(cmd);
    free(cmd);

    return bytevector;
}

SCM auxts_scm_execute(const char* cmd) {
    auxts_db* db = auxts_db_instance();
    auxts_result res = auxts_db_execute(db, cmd);

    SCM size = scm_from_uint64(res.size);
    SCM bytevector = scm_make_bytevector(size, SCM_UNDEFINED);
    memcpy(SCM_BYTEVECTOR_CONTENTS(bytevector), res.data, res.size);

    auxts_result_destroy(&res);
    return bytevector;
}

void auxts_scm_init_bindings() {
    scm_c_define_gsubr("extract", 3, 0, 0, auxts_scm_extract);
    scm_c_define_gsubr("create", 4, 0, 0, auxts_scm_create);
}