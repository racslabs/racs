
#ifndef RATS_STREAMINFO_H
#define RATS_STREAMINFO_H

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include "types.h"
#include "bytes.h"
#include "cache.h"
#include "filelist.h"

typedef struct {
    rats_uint16 channels;
    rats_uint16 bit_depth;
    rats_uint32 sample_rate;
    rats_uint64 size;
    rats_time   ref;
    rats_uint32 id_size;
    char*       id;
} rats_streaminfo;

typedef struct {
    char **streams;
    size_t num_streams;
    size_t max_streams;
} rats_streams;

rats_uint64 rats_streaminfo_attr(rats_cache *mcache, rats_uint64 stream_id, const char *attr);

int rats_streaminfo_get(rats_cache *mcache, rats_streaminfo *streaminfo, rats_uint64 stream_id);

int rats_streaminfo_put(rats_cache *mcache, rats_streaminfo *streaminfo, rats_uint64 stream_id);

void rats_streaminfo_load(rats_cache *mcache);

void rats_streaminfo_list(rats_cache *mcache, rats_streams *streams);

void rats_streams_add(rats_streams *streams, const char *stream);

size_t rats_streaminfo_size(rats_streaminfo* streaminfo);

off_t rats_streaminfo_write(rats_uint8 *buf, rats_streaminfo *streaminfo);

off_t rats_streaminfo_read(rats_streaminfo *streaminfo, rats_uint8 *buf);

size_t rats_streaminfo_filesize(const char *path);

void rats_streaminfo_flush(rats_uint8 *data, rats_uint32 len, rats_uint64 stream_id);

void rats_streaminfo_path(char *path, rats_uint64 stream_id);

int rats_streaminfo_exits(rats_uint64 stream_id);

rats_time rats_streaminfo_offset(rats_streaminfo *streaminfo);

void rats_mcache_destroy(void *key, void *value);

rats_cache *rats_mcache_create(size_t capacity);

rats_uint64 rats_hash(const char *stream_id);

#endif //RATS_STREAMINFO_H
