
#ifndef RACS_STREAMINFO_H
#define RACS_STREAMINFO_H

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fnmatch.h>
#include "types.h"
#include "bytes.h"
#include "cache.h"
#include "filelist.h"
#include "log.h"

#ifndef FNM_IGNORECASE
#define FNM_IGNORECASE 0x01
#endif

typedef struct {
    racs_uint16 channels;
    racs_uint16 bit_depth;
    racs_uint32 sample_rate;
    racs_uint64 size;
    racs_time   ref;
    racs_time   ttl;
    racs_uint32 id_size;
    char*       id;
} racs_streaminfo;

typedef struct {
    char **streams;
    size_t num_streams;
    size_t max_streams;
} racs_streams;

extern const char* racs_streaminfo_dir;

racs_uint64 racs_streaminfo_attr(racs_cache *mcache, racs_uint64 stream_id, const char *attr);

int racs_streaminfo_get(racs_cache *mcache, racs_streaminfo *streaminfo, racs_uint64 stream_id);

int racs_streaminfo_put(racs_cache *mcache, racs_streaminfo *streaminfo, racs_uint64 stream_id);

void racs_streaminfo_list(racs_cache *mcache, racs_streams *streams, const char* pattern);

size_t racs_streaminfo_size(racs_streaminfo* streaminfo);

off_t racs_streaminfo_write(racs_uint8 *buf, racs_streaminfo *streaminfo);

off_t racs_streaminfo_read(racs_streaminfo *streaminfo, racs_uint8 *buf);

size_t racs_streaminfo_filesize(const char *path);

void racs_streaminfo_flush(racs_uint8 *data, racs_uint32 len, racs_uint64 stream_id);

void racs_streaminfo_path(char **path, racs_uint64 stream_id);

int racs_streaminfo_exits(racs_uint64 stream_id);

racs_time racs_streaminfo_offset(racs_streaminfo *streaminfo);

void racs_streams_add(racs_streams *streams, const char *stream);

void racs_streams_init(racs_streams *streams);

void racs_streams_destroy(racs_streams *streams);

void racs_mcache_destroy(void *key, void *value);

racs_cache *racs_mcache_create(size_t capacity);

racs_uint64 racs_hash(const char *stream_id);

int racs_mcache_cmp(void *a, void *b);

racs_uint64 racs_path_to_stream_id(char *path);

#endif //RACS_STREAMINFO_H
