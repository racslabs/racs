#include "eval.h"


void racs_eval_expr(racs_ctx *ctx, msgpack_object obj);


void racs_eval(racs_ctx *ctx, const racs_uint8 *src, size_t size) {
    if (!ctx || ctx->has_error || !src || size == 0) {
        return;
    }

    msgpack_unpacked unpacked;
    msgpack_unpacked_init(&unpacked);

    size_t offset = 0;
    msgpack_unpack_return result = msgpack_unpack_next(&unpacked, (const char *) src, size, &offset);

    if (result != MSGPACK_UNPACK_SUCCESS && result != MSGPACK_UNPACK_EXTRA_BYTES) {
        ctx->has_error = 1;
        racs_pack_err(&ctx->out_buf, "error parsing msgpack");
        msgpack_unpacked_destroy(&unpacked);
        return;
    }

    racs_eval_expr(ctx, unpacked.data);
    msgpack_unpacked_destroy(&unpacked);
}

void racs_eval_expr(racs_ctx *ctx, msgpack_object obj) {
    if (ctx->has_error) {
        return;
    }

    if (obj.type != MSGPACK_OBJECT_ARRAY) {
        ctx->has_error = 1;
        racs_pack_err(&ctx->out_buf, "command expression must be an array");
        return;
    }

    if (obj.via.array.size == 0) {
        ctx->has_error = 1;
        racs_pack_err(&ctx->out_buf, "empty s-expression");
        return;
    }

    if (obj.via.array.ptr[0].type != MSGPACK_OBJECT_STR) {
        ctx->has_error = 1;
        racs_pack_err(&ctx->out_buf, "command must be a string");
        return;
    }

    const char *name = obj.via.array.ptr[0].via.str.ptr;
    size_t size = obj.via.array.ptr[0].via.str.size;
    char *s_name = strndup(name, size);

    racs_cmd_func func = racs_cmd_lookup(racs_trim(s_name));
    free(s_name);

    if (!func) {
        ctx->has_error = 1;
        char msg[255];
        snprintf(msg, sizeof(msg), "unknown command: %.*s", (int) size, name);
        racs_pack_err(&ctx->out_buf, msg);
        return;
    }

    size_t num_args = obj.via.array.size - 1;
    msgpack_object *args = &obj.via.array.ptr[1];
    
    func(ctx, args, num_args);
}