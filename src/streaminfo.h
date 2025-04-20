
#ifndef AUXTS_STREAMINFO_H
#define AUXTS_STREAMINFO_H

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include "types.h"
#include "bytes.h"
#include "cache.h"

typedef struct {
    auxts_uint16 channels;
    auxts_uint16 bit_depth;
    auxts_uint32 sample_rate;
    auxts_uint64 size;
    auxts_time   ref;
} auxts_streaminfo;

auxts_uint64 auxts_streaminfo_attr(auxts_cache* mcache, auxts_uint64 stream_id, const char* attr);
int auxts_streaminfo_get(auxts_cache* mcache, auxts_streaminfo* streaminfo, auxts_uint64 stream_id);
int auxts_streaminfo_put(auxts_cache* mcache, auxts_streaminfo* streaminfo, auxts_uint64 stream_id);
off_t auxts_streaminfo_write(auxts_uint8* buf, auxts_streaminfo* streaminfo);
off_t auxts_streaminfo_read(auxts_streaminfo* streaminfo, auxts_uint8* buf);
void auxts_streaminfo_flush(auxts_streaminfo* streaminfo, auxts_uint64 stream_id);
void auxts_streaminfo_path(char* path, auxts_uint64 stream_id);
int  auxts_streaminfo_exits(auxts_uint64 stream_id);
void auxts_mcache_destroy(void* key, void* value);
auxts_cache* auxts_mcache_create(size_t capacity);

#endif //AUXTS_STREAMINFO_H
