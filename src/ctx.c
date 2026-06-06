
#include "ctx.h"


void racs_ctx_init(racs_ctx *ctx) {
    if (!ctx) {
        return;
    }

    memset(ctx, 0, sizeof(racs_ctx));
    msgpack_sbuffer_init(&ctx->out_buf);
}

void racs_ctx_cleanup(racs_ctx *ctx) {
    if (!ctx) {
        return;
    }

    msgpack_sbuffer_destroy(&ctx->out_buf);
}
