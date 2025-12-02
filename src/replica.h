
#ifndef RACS_REPLICA_H
#define RACS_REPLICA_H

#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include "log.h"
#include "config.h"
#include "bytes.h"

#define RACS_MAX_REPLICAS 5

typedef struct {
    int fd;
} racs_replica;

typedef struct {
    int size;
    racs_replica replicas[RACS_MAX_REPLICAS];
} racs_replica_set;

void racs_replica_set_init(racs_replica_set *set, racs_config *cfg);

void racs_replica_open(racs_replica *replica, const char *host, int port);

void racs_replica_connect(racs_replica *replica, const char *host, int port);

void racs_replica_set_socketopts(racs_replica *replica);

void racs_replica_set_nonblocking(racs_replica *replica);

void racs_replica_init_socket(racs_replica *replica);

#endif //RACS_REPLICA_H