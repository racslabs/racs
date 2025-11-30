
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
#include "queue.h"

#define RACS_MAX_SLAVES 7

typedef struct {
    int fd;
    racs_queue q;
} racs_slave;

typedef struct {
    int size;
    racs_slave *slaves[RACS_MAX_SLAVES];
} racs_slaves;

void racs_slaves_init(racs_slaves *slaves);

void racs_slaves_add(racs_slaves *slaves, const char *host, int port);

void racs_slaves_broadcast(racs_slaves *slaves, const char *data, size_t size);

void *racs_slave_worker(void *arg);

racs_slave *racs_slave_open(const char *host, int port);

void racs_slave_connect(racs_slave *slave, const char *host, int port);

void racs_slave_set_socketopts(racs_slave *slave);

void racs_slave_init_socket(racs_slave *slave);

ssize_t racs_blocking_send(int fd, const char *data, size_t size);

#endif //RACS_SLAVE_H