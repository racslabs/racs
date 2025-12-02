
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
#include "config.h"

#define RACS_MAX_SLAVES 5

typedef struct {
    int fd;
} racs_slave;

void racs_slave_open(racs_slave *slave, const char *host, int port);

void racs_slave_connect(racs_slave *slave, const char *host, int port);

void racs_slave_set_socketopts(racs_slave *slave);

void racs_slave_set_nonblocking(racs_slave *slave);

void racs_slave_init_socket(racs_slave *slave);

#endif //RACS_SLAVE_H