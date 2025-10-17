<p align="center">
    <img width="300px" src="logo.svg"  alt=""/>
</p>

## What is RACS?

**RACS (Remote Audio Caching Server)** is an append-only, persistent audio caching server with built-in range queries, format conversion, and low-latency streaming.
Unlike traditional databases, **RACS** is purpose-built for storing, querying, and streaming audio data at scale.

### Key Features

- **Low-Latency Streaming**: Real-time delivery of audio directly from memory or disk, ideal for edge caches and application servers.
- **Format Conversion**: On-the-fly transcoding between PCM, MP3, Opus, and WAV formats to adapt to client or bandwidth requirements.
- **Time-Range Queries**: Retrieve precise audio segments by timestamp for efficient slicing, analysis, or playback.
- **Metadata Management**: Store and query audio-specific metadata such as sample rate, bit depth, and channel count.
- **Caching & Persistence**: Hybrid memory–disk storage with LRU caching for fast access and durable persistence.

### Storage Engine

**RACS** uses a multi-tiered architecture inspired by LSM (Log-Structured Merge) trees:

1. **Memtables**:
Incoming audio blocks are written in memory and batched for efficient disk flushes.
2. **SSTables (Time-Partitioned)**:
When memtables reach capacity, they are flushed as immutable time-partitioned files on disk. This layout enables fast range queries by timestamp.
3. **LRU Cache**:
Frequently accessed SSTables are cached in memory, minimizing disk I/O and improving response times.

This architecture ensures a balance between high write performance, query efficiency, and data durability.

## ⚠️ MVP Stage

**RACS** is currently in the **MVP (Minimum Viable Product)** stage. All the key features mentioned above are fully functional.

Planned enhancements include:
- **Replication:** Multi-node clustering for distributed storage and redundancy
- **Compaction:** Offline SSTable compaction to optimize disk usage
- **TTL Retention Policies:** Cleanup of expired audio data during compaction
- **Compression:** Reduced on-disk footprint for archived segments
- **DSP Functions:** Built-in signal processing functions


## Is RACS Open Source?

**RACS** is distributed under the RACS Source Available License (RACS-SAL-1.0).
It is free for personal, educational, and non-commercial use.

Commercial use requires a paid license.
For licensing or collaboration inquiries, contact sales@racslabs.com

## Getting Started

The quickest way to run RACS is using ``docker``.

If you do not have docker on your machine, [Install Docker](https://docs.docker.com/get-started/get-docker/)  before continuing.

```sh
    docker build --tag racslabs/racs .
```

```sh
    docker run --network=host -p 6381:6381 racslabs/racs
```

You can now connect to the server on port 6381.

Other Build Options
-------------------

- :ref:`Build From Source <BUILD_FROM_SOURCE>`


