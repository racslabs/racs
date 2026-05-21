#include "eval.h"


#define RACS_CHECK_ARG_TYPE(ctx, obj, offset, type, err_msg) \
do { \
    if (msgpack_unpack_next(&(obj), (ctx)->out_buf.data, (ctx)->out_buf.size, &(offset)) <= 0) { \
        (ctx)->has_error = 1; \
        racs_pack_err(&(ctx)->out_buf, "Missing arguments"); \
        msgpack_unpacked_destroy(&(obj)); \
        return; \
    } \
    if ((obj).data.type != (type)) { \
        (ctx)->has_error = 1; \
        racs_pack_err(&(ctx)->out_buf, (err_msg)); \
        msgpack_unpacked_destroy(&(obj)); \
        return; \
    } \
} while(0)


#define RACS_CHECK_ARG_COUNT(ctx, actual, expected, err_msg) \
do { \
    if ((actual) != (expected)) { \
        (ctx)->has_error = 1; \
        racs_pack_err(&(ctx)->out_buf, (err_msg)); \
        return; \
    } \
} while(0)

typedef void (*racs_cmd_func) (racs_eval_ctx *ctx, size_t num_args);

typedef struct {
    char u_name[55];
    char l_name[55];
    racs_cmd_func func;
} racs_cmd;


void racs_eval_node(racs_eval_ctx *ctx, msgpack_object obj);

racs_cmd_func racs_cmd_lookup(const char *name, size_t size);

// Command declarations

void racs_cmd_ping(racs_eval_ctx *ctx, size_t num_args);

const racs_cmd cmds[1] = {
    { "PING", "ping", racs_cmd_ping }
};


void racs_eval(racs_eval_ctx *ctx, const racs_uint8 *source, size_t size) {
    if (!ctx || ctx->has_error || !source || size == 0) {
        return;
    }

    msgpack_unpacked unpacked;
    msgpack_unpacked_init(&unpacked);

    size_t offset = 0;
    msgpack_unpack_return result = msgpack_unpack_next(&unpacked, (const char *)source, size, &offset);

    if (result != MSGPACK_UNPACK_SUCCESS && result != MSGPACK_UNPACK_EXTRA_BYTES) {
        ctx->has_error = 1;
        racs_pack_err(&ctx->out_buf, "MsgPack parsing failed");

        msgpack_unpacked_destroy(&unpacked);
        return;
    }

    racs_eval_node(ctx, unpacked.data);
    msgpack_unpacked_destroy(&unpacked);
}

void racs_eval_node(racs_eval_ctx *ctx, msgpack_object obj) {
    if (ctx->has_error) {
        return;
    }

    if (++ctx->depth > RACS_MAX_RECURSION_DEPTH) {
        ctx->has_error = 1;
        racs_pack_err(&ctx->out_buf, "Max recursion depth exceeded");
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
        racs_pack_err(&ctx->out_buf, "Empty S-expression");
        ctx->depth--;
        return;
    }

    if (obj.via.array.ptr[0].type != MSGPACK_OBJECT_STR) {
        ctx->has_error = 1;
        racs_pack_err(&ctx->out_buf, "Command must be a string");
        ctx->depth--;
        return;
    }

    const char *name = obj.via.array.ptr[0].via.str.ptr;
    size_t size = obj.via.array.ptr[0].via.str.size;

    racs_cmd_func func = racs_cmd_lookup(name, size);
    if (!func) {
        ctx->has_error = 1;

        char msg[255];
        snprintf(msg, sizeof(msg), "Unknown command: %.*s", (int)size, name);

        racs_pack_err(&ctx->out_buf, msg);
        ctx->depth--;
        return;
    }

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

void racs_eval_ctx_init(racs_eval_ctx *ctx) {
    if (!ctx) {
        return;
    }

    ctx->depth = 0;
    ctx->has_error = 0;

    ctx->sample_rate = 0;
    ctx->channels = 0;
    ctx->bit_depth = 0;

    msgpack_sbuffer_init(&ctx->out_buf);
}

void racs_eval_ctx_cleanup(racs_eval_ctx *ctx) {
    if (!ctx) {
        return;
    }

    msgpack_sbuffer_destroy(&ctx->out_buf);
}

racs_cmd_func racs_cmd_lookup(const char *name, size_t size) {
    size_t num_cmds = sizeof(cmds) / sizeof(cmds[0]);

    for (size_t i = 0; i < num_cmds; i++) {
        racs_cmd cmd = cmds[i];

        if ((size == strlen(cmd.u_name) && memcmp(cmd.u_name, name, size) == 0) ||
            (size == strlen(cmd.l_name) && memcmp(cmd.l_name, name, size) == 0)) {
            return cmd.func;
        }
    }
    return NULL;
}

// Command implementations

void racs_cmd_ping(racs_eval_ctx *ctx, size_t num_args) {
    RACS_CHECK_ARG_COUNT(ctx, num_args, 0, "PING requires 0 args");

    msgpack_packer pk;
    msgpack_packer_init(&pk, &ctx->out_buf, msgpack_sbuffer_write);
    msgpack_pack_str_with_body(&pk, "PONG", 4);
}
