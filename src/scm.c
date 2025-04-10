#include "scm.h"

void auxts_scm_propagate_error(msgpack_object* obj, uint8_t* data) {
    char* message = auxts_deserialize_str(obj, 1);
    SCM error = scm_from_utf8_string(message);

    free(data);
    free(message);

    scm_misc_error("extract", "~A", scm_list_1(error));
}

int auxts_scm_serialize_s8vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const int8_t* data = scm_s8vector_elements(v, &handle, &n, NULL);
    auxts_serialize_i8v(pk, (int8_t*)data, n);
    scm_array_handle_release (&handle);

    return AUXTS_STATUS_OK;
}

int auxts_scm_serialize_u8vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const uint8_t* data = scm_u8vector_elements(v, &handle, &n, NULL);
    auxts_serialize_u8v(pk, (uint8_t*)data, n);
    scm_array_handle_release (&handle);

    return AUXTS_STATUS_OK;
}

int auxts_scm_serialize_s16vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const int16_t* data = scm_s16vector_elements(v, &handle, &n, NULL);
    auxts_serialize_i16v(pk, (int16_t*)data, n);
    scm_array_handle_release (&handle);

    return AUXTS_STATUS_OK;
}

int auxts_scm_serialize_u16vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const uint16_t* data = scm_u16vector_elements(v, &handle, &n, NULL);
    auxts_serialize_u16v(pk, (uint16_t*)data, n);
    scm_array_handle_release (&handle);

    return AUXTS_STATUS_OK;
}

int auxts_scm_serialize_s32vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const int32_t* data = scm_s32vector_elements(v, &handle, &n, NULL);
    auxts_serialize_i32v(pk, (int32_t*)data, n);
    scm_array_handle_release (&handle);

    return AUXTS_STATUS_OK;
}

int auxts_scm_serialize_u32vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const uint32_t* data = scm_u32vector_elements(v, &handle, &n, NULL);
    auxts_serialize_u32v(pk, (uint32_t*)data, n);
    scm_array_handle_release (&handle);

    return AUXTS_STATUS_OK;
}

int auxts_scm_serialize_f32vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const float* data = scm_f32vector_elements(v, &handle, &n, NULL);
    auxts_serialize_f32v(pk, (float*)data, n);
    scm_array_handle_release (&handle);

    return AUXTS_STATUS_OK;
}

int auxts_scm_serialize_c32vector(msgpack_packer* pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const float* data = scm_c32vector_elements(v, &handle, &n, NULL);
    auxts_serialize_c64v(pk, (auxts_complex*)data, n);
    scm_array_handle_release (&handle);

    return AUXTS_STATUS_OK;
}

int auxts_scm_serialize(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x) {
    if (scm_is_integer(x))
        return auxts_serialize_int64(pk, scm_to_int64(x));
    if (scm_is_number(x))
        return auxts_serialize_float64(pk, scm_to_double(x));
    if (scm_is_bool(x))
        return auxts_serialize_bool(pk, scm_to_bool(x));
    if (scm_is_null_or_nil(x))
        return auxts_serialize_null_with_status_ok(pk);
    if (scm_is_pair(x))
        return auxts_scm_serialize_list(pk, buf, x);
    if (scm_is_string(x))
        return auxts_serialize_str(pk, scm_to_locale_string(x));
    if (scm_is_typed_array(x, scm_from_locale_symbol("s8")))
        return auxts_scm_serialize_s8vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("u8")))
        return auxts_scm_serialize_u8vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("s16")))
        return auxts_scm_serialize_s16vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("u16")))
        return auxts_scm_serialize_u16vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("s32")))
        return auxts_scm_serialize_s32vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("u32")))
        return auxts_scm_serialize_u32vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("f32")))
        return auxts_scm_serialize_f32vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("c32")))
        return auxts_scm_serialize_c32vector(pk, x);

    msgpack_sbuffer_clear(buf);
    return auxts_serialize_error(pk, "Unsupported SCM type");
}

int auxts_scm_serialize_element(msgpack_packer* pk, msgpack_sbuffer* buf, SCM v) {
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
        msgpack_pack_double(pk, scm_to_double(v));
        return true;
    }

    if (scm_is_bool(v)) {
        scm_to_bool(v) ? msgpack_pack_true(pk) : msgpack_pack_false(pk);
        return true;
    }

    msgpack_sbuffer_clear(buf);
    auxts_serialize_error(pk, "Unsupported SCM type");

    return false;
}

int auxts_scm_serialize_list(msgpack_packer* pk, msgpack_sbuffer* buf, SCM x) {
    uint32_t n = scm_to_uint32(scm_length(x));

    msgpack_pack_array(pk, n + 1);
    auxts_serialize_type(pk, AUXTS_TYPE_LIST);

    while (scm_is_pair(x)) {
        SCM v = scm_car(x);

        if (!auxts_scm_serialize_element(pk, buf, v))
            return AUXTS_STATUS_ERROR;

        x = scm_cdr(x);
    }

    return AUXTS_STATUS_OK;
}

SCM auxts_scm_eval_wrapper(void* data) {
    return scm_c_eval_string((const char*)data);
}

SCM auxts_scm_error_handler(void* data, SCM key, SCM args) {
    scm_display(args, data);
    return SCM_BOOL_F;
}

SCM auxts_scm_eval_with_error_handling(char* expr, char** error) {
    SCM original_error_port = scm_current_error_port();
    SCM error_port = scm_open_output_string();

    scm_set_current_error_port(error_port);
    auxts_unescape_single_quotes(expr);

    SCM result = scm_c_catch(SCM_BOOL_T,
                             auxts_scm_eval_wrapper,
                             (void*)expr,
                             auxts_scm_error_handler,
                             error_port,
                             NULL,
                             NULL);

    if (SCM_BOOL_F == result) {
        SCM error_output = scm_get_output_string(error_port);
        *error = scm_to_locale_string(error_output);
    }

    scm_set_current_error_port(original_error_port);
    scm_close_output_port(error_port);

    return result;
}

void auxts_unescape_single_quotes(char* s) {
    size_t n = strlen(s);

    for (int i = 0; i < n - 1; ++i) {
        if (s[i] == '\\' && s[i + 1] == '\'') s[i] = ' ';
    }
}
