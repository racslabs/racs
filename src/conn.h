
#ifndef RACS_CONN_H
#define RACS_CONN_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <netinet/tcp.h>

typedef struct {
    int listen_sd, new_sd;
} racs_conn;

void racs_init_socketopts(racs_conn *conn);

#endif //RACS_CONN_H
