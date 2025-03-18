#include "scm.h"

SCM auxts_scm_extract(SCM scm_stream_id, SCM scm_from, SCM scm_to) {
    const char *from = scm_to_locale_string(scm_from);
    const char *to = scm_to_locale_string(scm_to);

    char *cmd = NULL;
    if (scm_is_integer(scm_stream_id)) {
        uint64_t stream_id = scm_to_uint64(scm_stream_id);
        asprintf(&cmd, "EXTRACT %llu '%s' '%s'", stream_id, from, to);
    } else {
        const char *stream_id = scm_to_locale_string(scm_stream_id);
        asprintf(&cmd, "EXTRACT '%s' '%s' '%s'", stream_id, from, to);
    }

    auxts_db* db = auxts_db_instance();
    auxts_result res = auxts_db_execute(db, cmd);

    SCM size = scm_from_uint64(res.size);
    SCM bytevector = scm_make_bytevector(size, SCM_UNDEFINED);
    memcpy(SCM_BYTEVECTOR_CONTENTS(bytevector), res.data, res.size);

    free(cmd);
    free(res.data);

    return bytevector;
}

void auxts_init_scm_bindings() {
    scm_c_define_gsubr("extract", 3, 0, 0, auxts_scm_extract);
}