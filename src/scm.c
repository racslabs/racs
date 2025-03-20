#include "scm.h"

SCM auxts_scm_extract(SCM stream_id, SCM from, SCM to) {
    char* cmd = NULL;
    asprintf(&cmd, "EXTRACT '%s' '%s' '%s'",
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
        char* message = auxts_deserialize_str(&msg.data, 1);
        SCM error = scm_from_utf8_string(message);

        free(res.data);
        free(message);

        scm_misc_error("extract", "~A", scm_list_1(error));
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

    SCM bytevector = auxts_scm_execute(cmd);
    free(cmd);

    return bytevector;
}

void auxts_scm_serialize_u8vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const uint8_t* data = scm_u8vector_elements(v, &handle, &n, NULL);
    auxts_serialize_bin(pk, (uint8_t*)data, n);
    scm_array_handle_release (&handle);
}

void auxts_scm_serialize_s16vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const int16_t* data = scm_s16vector_elements(v, &handle, &n, NULL);
    auxts_serialize_i16v(pk, (int16_t*)data, n);
    scm_array_handle_release (&handle);
}

void auxts_scm_serialize_u16vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const uint16_t* data = scm_u16vector_elements(v, &handle, &n, NULL);
    auxts_serialize_u16v(pk, (uint16_t*)data, n);
    scm_array_handle_release (&handle);
}

void auxts_scm_serialize_s32vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const int32_t* data = scm_s32vector_elements(v, &handle, &n, NULL);
    auxts_serialize_i32v(pk, (int32_t*)data, n);
    scm_array_handle_release (&handle);
}

void auxts_scm_serialize_u32vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const uint32_t* data = scm_u32vector_elements(v, &handle, &n, NULL);
    auxts_serialize_u32v(pk, (uint32_t*)data, n);
    scm_array_handle_release (&handle);
}

void auxts_scm_serialize_f32vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const float* data = scm_f32vector_elements(v, &handle, &n, NULL);
    auxts_serialize_f32v(pk, (float*)data, n);
    scm_array_handle_release (&handle);
}

void auxts_scm_serialize_c32vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const float* data = scm_c32vector_elements(v, &handle, &n, NULL);
    auxts_serialize_c64v(pk, (auxts_complex_t*)data, n);
    scm_array_handle_release (&handle);
}

void auxts_scm_serialize(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x) {
    if (scm_is_integer(x)) {
        auxts_serialize_int64(pk, scm_to_int64(x));
        return;
    }

    if (scm_is_number(x)) {
        auxts_serialize_float(pk, (float)scm_to_double(x));
        return;
    }

    if (scm_is_bool(x)) {
        auxts_serialize_bool(pk, scm_to_bool(x));
        return;
    }

    if (scm_is_null_or_nil(x)) {
        auxts_pack_none_with_status_ok(pk);
        return;
    }

    if (scm_is_pair(x)) {
        auxts_scm_serialize_list(pk, buf, x);
        return;
    }

    if (scm_is_string(x)) {
        auxts_serialize_str(pk, scm_to_locale_string(x));
        return;
    }

    if (scm_is_typed_array(x, scm_from_locale_symbol("u8"))) {
        auxts_scm_serialize_u8vector(pk, x);
        return;
    }

    if (scm_is_typed_array(x, scm_from_locale_symbol("s16"))) {
        auxts_scm_serialize_s16vector(pk, x);
        return;
    }

    if (scm_is_typed_array(x, scm_from_locale_symbol("u16"))) {
        auxts_scm_serialize_u16vector(pk, x);
        return;
    }

    if (scm_is_typed_array(x, scm_from_locale_symbol("s32"))) {
        auxts_scm_serialize_s32vector(pk, x);
        return;
    }

    if (scm_is_typed_array(x, scm_from_locale_symbol("u32"))) {
        auxts_scm_serialize_u32vector(pk, x);
        return;
    }

    if (scm_is_typed_array(x, scm_from_locale_symbol("f32"))) {
        auxts_scm_serialize_f32vector(pk, x);
        return;
    }

    if (scm_is_typed_array(x, scm_from_locale_symbol("c32"))) {
        auxts_scm_serialize_c32vector(pk, x);
        return;
    }

    msgpack_sbuffer_clear(buf);
    auxts_serialize_error(pk, "Unsupported SCM type");
}

int auxts_scm_serialize_element(msgpack_packer* pk,  msgpack_sbuffer* buf, SCM v) {
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


    msgpack_sbuffer_clear(buf);
    auxts_serialize_error(pk, "Unsupported SCM type");

    return false;
}

void auxts_scm_serialize_list(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x) {
    uint32_t n = scm_to_uint32(scm_length(x));

    msgpack_pack_array(pk, n + 1);
    auxts_serialize_type(pk, AUXTS_TYPE_LIST);

    while (scm_is_pair(x)) {
        SCM v = scm_car(x);

        if (!auxts_scm_serialize_element(pk, buf, v))
            break;

        x = scm_cdr(x);
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