
#ifndef RACS_SLAVE_H
#define RACS_SLAVE_H

#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "log.h"

typedef struct {
    int size;
    int fds[200];
} racs_slaves;

typedef struct {
    char *data;
    size_t length;
    int fd;
} racs_slave_worker_arg;

void racs_slaves_init(racs_slaves *slaves);

void racs_slaves_add(racs_slaves *slaves, const char *host, int port);

void racs_slaves_dispatch(racs_slaves *slaves, char *data, size_t length);

void racs_slave_worker(racs_slave_worker_arg *arg);

int racs_slave_open(const char *host, int port);

void racs_slave_connect(int fd, const char *host, int port);

void racs_slave_set_socketopts(int fd);

int racs_slave_init_socket();

size_t racs_slave_send(int fd, const char *data, size_t size);

#endif //RACS_SLAVE_H