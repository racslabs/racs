
#ifndef RATS_CONN_H
#define RATS_CONN_H

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
    int    close;
    char   buffer[80];
    struct sockaddr_in6   addr;
    int    timeout;
    struct pollfd fds[200];
    int    nfds, current_size, i, j;
} rats_conn;

void rats_conn_init(rats_conn* conn, int port);
void rats_conn_init_socket(rats_conn* conn);
void rats_conn_init_sockopt(rats_conn* conn);
void rats_conn_init_ioctl(rats_conn* conn);
void rats_conn_bind(rats_conn* conn, int port);
void rats_conn_listen(rats_conn* conn);
void rats_conn_init_fds(rats_conn* conn);

#endif //RATS_CONN_H
