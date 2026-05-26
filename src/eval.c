#include "eval.h"


#define RACS_UNPACK_ARG(ctx, unpacked, offset, err_msg) \
do { \
    if (msgpack_unpack_next(&(unpacked), (ctx)->out_buf.data, (ctx)->out_buf.size, &(offset)) <= 0) { \
        RACS_PACK_ERR((ctx), (unpacked), (err_msg)); \
    } \
} while(0)


#define RACS_CHECK_STR(ctx, unpacked, err_msg) \
do { \
    if ((unpacked).data.type != MSGPACK_OBJECT_STR) { \
        RACS_PACK_ERR((ctx), (unpacked), (err_msg)); \
    } \
} while(0)


#define RACS_CHECK_INT(ctx, unpacked, err_msg) \
do { \
    if ((unpacked).data.type != MSGPACK_OBJECT_POSITIVE_INTEGER) { \
        RACS_PACK_ERR((ctx), (unpacked), (err_msg)); \
    } \
} while(0)


#define RACS_PACK_ERR(ctx, unpacked, err_msg) \
do { \
    (ctx)->has_error = 1; \
    racs_pack_err(&(ctx)->out_buf, (err_msg)); \
    msgpack_unpacked_destroy(&(unpacked)); \
    return; \
} while(0)


#define RACS_COUNT_ARGS(ctx, actual, expected, err_msg) \
do { \
    if ((actual) != (expected)) { \
        (ctx)->has_error = 1; \
        racs_pack_err(&(ctx)->out_buf, (err_msg)); \
        return; \
    } \
} while(0)


typedef void (*racs_cmd_func) (racs_eval_ctx *ctx, size_t num_args);

typedef struct {
    char name[55];
    racs_cmd_func func;
} racs_cmd;


void racs_eval_node(racs_eval_ctx *ctx, msgpack_object obj);

racs_cmd_func racs_cmd_lookup(const char *name, size_t size);

// Command declarations

void racs_cmd_ping(racs_eval_ctx *ctx, size_t num_args);

void racs_cmd_create(racs_eval_ctx *ctx, size_t num_args);


const racs_cmd cmds[2] = {
    { "ping"  , racs_cmd_ping },
    { "create", racs_cmd_create }
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

        if (size == strlen(cmd.name) && memcmp(cmd.name, name, size) == 0) {
            return cmd.func;
        }
    }

    return NULL;
}


// Command implementations

void racs_cmd_ping(racs_eval_ctx *ctx, size_t num_args) {
    RACS_COUNT_ARGS(ctx, num_args, 0, "PING requires 0 args");

    msgpack_sbuffer_clear(&ctx->out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &ctx->out_buf, msgpack_sbuffer_write);
    msgpack_pack_str_with_body(&pk, "pong", 4);
}

void racs_cmd_create(racs_eval_ctx *ctx, size_t num_args) {
    RACS_COUNT_ARGS(ctx, num_args, 4, "CREATE requires 4 args");

    size_t offset = 0;
    msgpack_unpacked unpacked;
    msgpack_unpacked_init(&unpacked);

    RACS_UNPACK_ARG(ctx, unpacked, offset, "CREATE error unpacking arg1");
    RACS_CHECK_STR(ctx, unpacked, "CREATE expected string at arg1");

    const char *name = unpacked.data.via.str.ptr;
    size_t size = unpacked.data.via.str.size;

    char path[PATH_MAX];
    sprintf(path, "%s/.racs/md/", racs_config_get()->data_dir);
    strncat(path, name, size);

    RACS_UNPACK_ARG(ctx, unpacked, offset, "CREATE error unpacking arg2");
    RACS_CHECK_INT(ctx, unpacked, "CREATE expected int at arg2");

    racs_uint32 sample_rate = (racs_uint32) unpacked.data.via.u64;

    RACS_UNPACK_ARG(ctx, unpacked, offset, "CREATE error unpacking arg3");
    RACS_CHECK_INT(ctx, unpacked, "CREATE expected int at arg3");

    racs_uint8 channels = (racs_uint8) unpacked.data.via.u64;

    RACS_UNPACK_ARG(ctx, unpacked, offset, "CREATE error unpacking arg4");
    RACS_CHECK_INT(ctx, unpacked, "CREATE expected int at arg4");
    
    racs_uint8 bit_depth = (racs_uint8) unpacked.data.via.u64;

    if (racs_info_exist(path)) {
        RACS_PACK_ERR(ctx, unpacked, "CREATE stream already exist");
    }

    racs_create(path, sample_rate, channels, bit_depth);
    msgpack_sbuffer_clear(&ctx->out_buf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &ctx->out_buf, msgpack_sbuffer_write);
    msgpack_pack_nil(&pk);
}
