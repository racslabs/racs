
#ifndef RACS_TTL_H
#define RACS_TTL_H

#define _GNU_SOURCE
#include <ftw.h>

#include "offsets.h"
#include "metadata.h"

void racs_ttl(racs_offsets *offset);

void racs_ttl_async(racs_offsets *offset);

void *racs_ttl_worker(void *arg);

int racs_ttl_expire(racs_uint64 stream_id, racs_time ttl);

int racs_ttl_is_expired(racs_time ttl);

void racs_ttl_delete_stream(racs_offsets *offsets, racs_uint64 stream_id);

int racs_remove(const char *path);

#endif //RACS_TTL_H
