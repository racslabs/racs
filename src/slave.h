
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

int racs_slave_open(const char *host, int port);

void racs_slave_connect(int fd, const char *host, int port);

void racs_slave_set_socketopts(int fd);

int racs_slave_init_socket();

size_t racs_slave_send(int fd, const char *data, size_t size);

#endif //RACS_SLAVE_H