
#include "server.h"


void racs_read_cb(struct bufferevent *bev, void *data);


void racs_read_cb(struct bufferevent *bev, void *data) {
    (void) data; 

    struct evbuffer *in  = bufferevent_get_input(bev);
    struct evbuffer *out = bufferevent_get_output(bev);

    while (1) {
        size_t available = evbuffer_get_length(in);

        if (available < sizeof(uint64_t)) {
            return; 
        }

        uint64_t payload_len = 0;
        evbuffer_copyout(in, &payload_len, sizeof(uint64_t));

        if (available < sizeof(uint64_t) + payload_len) {
            return; 
        }

        evbuffer_drain(in, sizeof(uint64_t));

        racs_uint8 *buf = malloc(payload_len);
        if (!buf) {
            return;
        }

        evbuffer_remove(in, buf, payload_len);

        racs_ctx ctx;
        racs_ctx_init(&ctx);

        ctx.fd = bufferevent_getfd(bev);

        racs_eval(&ctx, buf, payload_len);

        free(buf);

        uint64_t out_len = (uint64_t)ctx.out_buf.size;
        evbuffer_add(out, &out_len, sizeof(uint64_t));

        if (ctx.out_buf.data && ctx.out_buf.size > 0) {
            evbuffer_add(out, ctx.out_buf.data, ctx.out_buf.size);
        }

        racs_ctx_cleanup(&ctx);
    }
}
