
#ifndef AUXTS_EXTRACT0_H
#define AUXTS_EXTRACT0_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <msgpack.h>
#include "memtable.h"
#include "filelist.h"
#include "result.h"
#include "context.h"
#include "flac0.h"
#include "metadata.h"

int auxts_extract_pcm(auxts_cache* cache, auxts_pcm* pcm, auxts_uint64 stream_id, auxts_time from, auxts_time to);


#endif //AUXTS_EXTRACT0_H
