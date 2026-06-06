#include "eval.h"


void racs_eval_node(racs_ctx *ctx, msgpack_object obj);


void racs_eval(racs_ctx *ctx, const racs_uint8 *src, size_t size) {
    if (!ctx || ctx->has_error || !src || size == 0) {
        return;
    }

    msgpack_unpacked unpacked;
    msgpack_unpacked_init(&unpacked);

    size_t offset = 0;
    msgpack_unpack_return result = msgpack_unpack_next(&unpacked, (const char *)src, size, &offset);

    if (result != MSGPACK_UNPACK_SUCCESS && result != MSGPACK_UNPACK_EXTRA_BYTES) {
        ctx->has_error = 1;
        racs_pack_err(&ctx->out_buf, "error parsing msgpack");

        msgpack_unpacked_destroy(&unpacked);
        return;
    }

    racs_eval_node(ctx, unpacked.data);
    msgpack_unpacked_destroy(&unpacked);
}

void racs_eval_node(racs_ctx *ctx, msgpack_object obj) {
    if (ctx->has_error) {
        return;
    }

    if (++ctx->depth > RACS_MAX_RECURSION_DEPTH) {
        ctx->has_error = 1;
        racs_pack_err(&ctx->out_buf, "max recursion depth exceeded");
        ctx->depth--;
        return;
    }

    if (obj.type != MSGPACK_OBJECT_ARRAY) {
        racs_pack_obj(&ctx->out_buf, obj);
        ctx->depth--;
        return;
    }

    if (obj.via.array.size == 0) {
        ctx->has_error = 1;
        racs_pack_err(&ctx->out_buf, "empty s-expression");
        ctx->depth--;
        return;
    }

    if (obj.via.array.ptr[0].type != MSGPACK_OBJECT_STR) {
        ctx->has_error = 1;
        racs_pack_err(&ctx->out_buf, "command must be a string");
        ctx->depth--;
        return;
    }

    const char *name = obj.via.array.ptr[0].via.str.ptr;
    size_t size = obj.via.array.ptr[0].via.str.size;
    char *s_name = strndup(name, size);

    racs_cmd_func func = racs_cmd_lookup(s_name);
    if (!func) {
        free(s_name);
        ctx->has_error = 1;

        char msg[255];
        snprintf(msg, sizeof(msg), "unknown command: %.*s", (int)size, name);

        racs_pack_err(&ctx->out_buf, msg);
        ctx->depth--;
        return;
    }

    free(s_name);
    size_t num_args = obj.via.array.size - 1;

    for (size_t i = 0; i < num_args; i++) {
        racs_eval_node(ctx, obj.via.array.ptr[i + 1]);
        if (ctx->has_error) {
            ctx->depth--;
            return;
        }
    }

    func(ctx, num_args);
    ctx->depth--;
}
