
#ifndef AUXTS_CONN_H
#define AUXTS_CONN_H

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

typedef struct {
    int    len, rc, on;
    int    listen_sd, new_sd;
    int    desc_ready, end_server, compress_array;
    int    close_conn;
    char   buffer[80];
    struct sockaddr_in6   addr;
    int    timeout;
    struct pollfd fds[200];
    int    nfds, current_size, i, j;
} auxts_conn;

void auxts_conn_init(auxts_conn* conn, int port);
void auxts_conn_init_socket(auxts_conn* conn);
void auxts_conn_init_sockopt(auxts_conn* conn);
void auxts_conn_init_ioctl(auxts_conn* conn);
void auxts_conn_bind(auxts_conn* conn, int port);
void auxts_conn_listen(auxts_conn* conn);
void auxts_conn_init_fds(auxts_conn* conn);

#endif //AUXTS_CONN_H
