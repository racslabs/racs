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

void auxts_scm_serialize(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x) {
    if (scm_is_string(x)) {
        auxts_serialize_str(pk, scm_to_locale_string(x));
        return;
    }

    if (scm_is_integer(x)) {
        auxts_serialize_int64(pk, scm_to_int64(x));
        return;
    }

    if (scm_is_number(x)) {
        auxts_serialize_float32(pk, (float)scm_to_double(x));
        return;
    }

    if (scm_is_pair(x)) {
        auxts_scm_serialize_list(pk, buf, x);
        return;
    }

    if (scm_is_bytevector(x)) {
        uint8_t* data = (uint8_t*)SCM_BYTEVECTOR_CONTENTS(x);
        size_t size = SCM_BYTEVECTOR_LENGTH(x);
        auxts_serialize_bin(pk, data, size);
        return;
    }

    msgpack_sbuffer_clear(buf);
    auxts_serialize_error(pk, "Unsupported SCM type");
}

int auxts_scm_serialize_element(msgpack_packer* pk, SCM v) {
    if (scm_is_string(v)) {
        char* str = scm_to_locale_string(v);
        msgpack_pack_str_with_body(pk, str, strlen(str));
        return true;
    }

    if (scm_is_integer(v)) {
        msgpack_pack_int64(pk, scm_to_int64(v));
        return true;
    }

    if (scm_is_number(v)) {
        msgpack_pack_float(pk, (float)scm_to_double(v));
        return true;
    }

    if (scm_is_bytevector(v)) {
        uint8_t* data = (uint8_t*)SCM_BYTEVECTOR_CONTENTS(v);
        size_t size = SCM_BYTEVECTOR_LENGTH(v);
        msgpack_pack_bin_with_body(pk, data, size);
        return true;
    }

    return false;
}

void auxts_scm_serialize_list(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x) {
    uint32_t n = scm_to_uint32(scm_length(x));

    auxts_serialize_type(pk, AUXTS_TYPE_LIST);
    msgpack_pack_array(pk, n);

    while (scm_is_pair(x)) {
        SCM v = scm_car(x);

        if (auxts_scm_serialize_element(pk, v)) {
            x = scm_cdr(x);
            continue;
        }

        msgpack_sbuffer_clear(buf);
        auxts_serialize_error(pk, "Unsupported SCM type");
        break;
    }
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