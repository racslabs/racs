// RACS - Remote Audio Caching Server
// Copyright (c) 2025 RACS Labs, LLC. All rights reserved.
//
// Licensed under the RACS Source Available License (RACS-SAL-1.0).
// Non-commercial use only. Commercial use requires a paid license.
// Contact: sales@racslabs.com
//
// SPDX-License-Identifier: RACS-SAL-1.0

#include "scm.h"

void racs_scm_propagate_error(msgpack_object *obj, racs_uint8 *data) {
    char *message = racs_unpack_str(obj, 1);
    SCM error = scm_from_utf8_string(message);

    free(data);
    free(message);

    scm_misc_error("", "~A", scm_list_1(error));
}

int racs_scm_pack_s8vector(msgpack_packer *pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const racs_int8 *data = scm_s8vector_elements(v, &handle, &n, NULL);
    racs_pack_s8v(pk, (racs_int8 *) data, n);
    scm_array_handle_release(&handle);

    return RACS_STATUS_OK;
}

int racs_scm_pack_u8vector(msgpack_packer *pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const racs_uint8 *data = scm_u8vector_elements(v, &handle, &n, NULL);
    racs_pack_u8v(pk, (racs_uint8 *) data, n);
    scm_array_handle_release(&handle);

    return RACS_STATUS_OK;
}

int racs_scm_pack_s16vector(msgpack_packer *pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const racs_int16 *data = scm_s16vector_elements(v, &handle, &n, NULL);
    racs_pack_s16v(pk, (racs_int16 *) data, n);
    scm_array_handle_release(&handle);

    return RACS_STATUS_OK;
}

int racs_scm_pack_u16vector(msgpack_packer *pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const racs_uint16 *data = scm_u16vector_elements(v, &handle, &n, NULL);
    racs_pack_u16v(pk, (racs_uint16 *) data, n);
    scm_array_handle_release(&handle);

    return RACS_STATUS_OK;
}

int racs_scm_pack_s32vector(msgpack_packer *pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const racs_int32 *data = scm_s32vector_elements(v, &handle, &n, NULL);
    if (!data)
        racs_pack_s32v(pk, 0, 0);
    else
        racs_pack_s32v(pk, (racs_int32 *) data, n);

    scm_array_handle_release(&handle);
    return RACS_STATUS_OK;
}

int racs_scm_pack_u32vector(msgpack_packer *pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const racs_uint32 *data = scm_u32vector_elements(v, &handle, &n, NULL);
    racs_pack_u32v(pk, (racs_uint32 *) data, n);
    scm_array_handle_release(&handle);

    return RACS_STATUS_OK;
}

int racs_scm_pack_f32vector(msgpack_packer *pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const float *data = scm_f32vector_elements(v, &handle, &n, NULL);
    racs_pack_f32v(pk, (float *) data, n);
    scm_array_handle_release(&handle);

    return RACS_STATUS_OK;
}

int racs_scm_pack_c32vector(msgpack_packer *pk, SCM v) {
    scm_t_array_handle handle;
    size_t n;

    const float *data = scm_c32vector_elements(v, &handle, &n, NULL);
    racs_pack_c64v(pk, (racs_complex *) data, n);
    scm_array_handle_release(&handle);

    return RACS_STATUS_OK;
}

int racs_scm_pack(msgpack_packer *pk, msgpack_sbuffer *buf, SCM x) {
    if (scm_is_integer(x))
        return racs_pack_int64(pk, scm_to_int64(x));
    if (scm_is_number(x))
        return racs_pack_float64(pk, scm_to_double(x));
    if (scm_is_bool(x))
        return racs_pack_bool(pk, scm_to_bool(x));
    if (scm_is_null_or_nil(x))
        return racs_pack_null_with_status_ok(pk);
    if (scm_is_pair(x))
        return racs_scm_pack_list(pk, buf, x);
    if (scm_is_string(x))
        return racs_pack_str(pk, scm_to_locale_string(x));
    if (scm_is_typed_array(x, scm_from_locale_symbol("s8")))
        return racs_scm_pack_s8vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("u8")))
        return racs_scm_pack_u8vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("s16")))
        return racs_scm_pack_s16vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("u16")))
        return racs_scm_pack_u16vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("s32")))
        return racs_scm_pack_s32vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("u32")))
        return racs_scm_pack_u32vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("f32")))
        return racs_scm_pack_f32vector(pk, x);
    if (scm_is_typed_array(x, scm_from_locale_symbol("c32")))
        return racs_scm_pack_c32vector(pk, x);

    msgpack_sbuffer_clear(buf);
    return racs_pack_error(pk, "Unsupported SCM type");
}

int racs_scm_pack_element(msgpack_packer *pk, msgpack_sbuffer *buf, SCM v) {
    if (scm_is_string(v)) {
        char *str = scm_to_locale_string(v);
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
    racs_pack_error(pk, "Unsupported SCM type");

    return false;
}

int racs_scm_pack_list(msgpack_packer *pk, msgpack_sbuffer *buf, SCM x) {
    racs_uint32 n = scm_to_uint32(scm_length(x));

    msgpack_pack_array(pk, n + 1);
    racs_pack_type(pk, RACS_TYPE_LIST);

    while (scm_is_pair(x)) {
        SCM v = scm_car(x);

        if (!racs_scm_pack_element(pk, buf, v))
            return RACS_STATUS_ERROR;

        x = scm_cdr(x);
    }

    return RACS_STATUS_OK;
}

SCM racs_scm_safe_eval(void *body) {
    scm_c_use_module("ice-9 sandbox");

    SCM eval_in_sandbox = scm_variable_ref(scm_c_lookup("eval-in-sandbox"));
    SCM make_sandbox_module = scm_variable_ref(scm_c_lookup("make-sandbox-module"));

    SCM base_module = scm_list_4(scm_list_2(scm_from_locale_symbol("scheme"),
                                            scm_from_locale_symbol("base")),
                                 scm_from_locale_symbol("+"),
                                 scm_from_locale_symbol("-"),
                                 scm_from_locale_symbol("*"));

    SCM racs_module = scm_list_n(scm_list_1(scm_from_locale_symbol("racs")),
                                 scm_from_locale_symbol("range"),
                                 scm_from_locale_symbol("meta"),
                                 scm_from_locale_symbol("encode"),
                                 scm_from_locale_symbol("list"),
                                 scm_from_locale_symbol("mix"),
                                 scm_from_locale_symbol("gain"),
                                 scm_from_locale_symbol("trim"),
                                 scm_from_locale_symbol("fade"),
                                 scm_from_locale_symbol("pan"),
                                 scm_from_locale_symbol("pad"),
                                 scm_from_locale_symbol("clip"),
                                 scm_from_locale_symbol("split"),
                                 scm_from_locale_symbol("merge"),
                                 SCM_UNDEFINED);

    SCM modules = scm_list_2(base_module, racs_module);
    SCM bindings = scm_call_1(make_sandbox_module, modules);

    SCM kw_module = scm_from_latin1_keyword("module");
    SCM kw_time_limit = scm_from_latin1_keyword("time-limit");
    SCM kw_alloc_limit = scm_from_latin1_keyword("allocation-limit");

    SCM time_limit = scm_from_double(10.0);                // 10 seconds
    SCM alloc_limit = scm_from_size_t(50 * 1024 * 1024);    // ~50 MB

    SCM expr = scm_c_read_string((char *)body);

    return scm_call_7(eval_in_sandbox,
                      expr,
                      kw_module, bindings,
                      kw_time_limit, time_limit,
                      kw_alloc_limit, alloc_limit);
}


SCM racs_scm_error_handler(void *data, SCM key, SCM args) {
    scm_display(args, data);
    return SCM_BOOL_F;
}

SCM racs_scm_safe_eval_with_error_handling(char *expr, char **error) {
    SCM original_error_port = scm_current_error_port();
    SCM error_port = scm_open_output_string();

    scm_set_current_error_port(error_port);
    racs_unescape_single_quotes(expr);

    SCM result = scm_c_catch(SCM_BOOL_T,
                             racs_scm_safe_eval,
                             expr,
                             racs_scm_error_handler,
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

void racs_unescape_single_quotes(char *s) {
    size_t n = strlen(s);

    for (int i = 0; i < n - 1; ++i) {
        if (s[i] == '\\' && s[i + 1] == '\'') s[i] = ' ';
    }
}
