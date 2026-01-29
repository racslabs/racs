
#ifndef RACS_TTL_H
#define RACS_TTL_H

#define _GNU_SOURCE
#include <ftw.h>

#include "metadata.h"

void racs_ttl();

void racs_ttl_async();

void *racs_ttl_worker(void *arg);

int racs_ttl_expire(racs_uint64 stream_id, racs_time ttl);

int racs_ttl_is_expired(racs_time ttl);

void racs_ttl_delete_stream(racs_uint64 stream_id);

int racs_remove(const char *path);

#endif //RACS_TTL_H
