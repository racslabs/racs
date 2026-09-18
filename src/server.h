
#ifndef RACS_SERVER_H
#define RACS_SERVER_H

#ifdef __cplusplus
extern "C" {
#endif


#include "eval.h"
#include <signal.h>
#include <arpa/inet.h>
#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


typedef struct racs_server_context {
    struct event_base *base;
    struct evconnlistener *listener;
    int port;
} racs_server_ctx;


#ifdef __cplusplus
}
#endif

#endif //RACS_SERVER_H