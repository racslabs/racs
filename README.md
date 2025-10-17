<p align="center">
    <img width="300px" src="logo.svg"  alt=""/>
</p>

## What is RACS?

**RACS (Remote Audio Caching Server)** is an append-only, persistent audio caching server with built-in range queries, format conversion, and low-latency streaming.


## Key Features

- **Storage & Archival:** Cache and persist large volumes of audio data efficiently
- **Streaming:** Stream raw PCM data with low latency
- **Format Conversion:** On-the-fly conversion between PCM, MP3, Opus, and WAV formats
- **Query Engine:** Execute range queries on time-series audio data
- **Metadata Management:** Manage audio metadata attributes – i.e., sample rate, channels, etc.
- **Programmability:** Execute scheme expressions directly on the server


## Storage Engine

**RACS** uses a multi-tiered architecture inspired by LSM (Log-Structured Merge) trees:

1. **Memtables**:
Incoming audio blocks are written in memory and batched for efficient disk flushes.
2. **SSTables (Time-Partitioned)**:
When memtables reach capacity, they are flushed as immutable time-partitioned files on disk. This layout enables fast range queries by timestamp.
3. **LRU Cache**:
Frequently accessed SSTables are cached in memory, minimizing disk I/O and improving response times.

This architecture ensures a balance between high write performance, query efficiency, and data durability.


## Is RACS Open Source?

**RACS** is distributed under the RACS Source Available License (RACS-SAL-1.0).
It is free for personal, educational, and non-commercial use.

Commercial use requires a paid license.
For licensing or collaboration inquiries, contact sales@racslabs.com


## ⚠️ MVP Stage

**RACS** is currently in the **MVP (Minimum Viable Product)** stage.  
All core components are functional, including audio caching, low-latency streaming, format conversion, range queries, and Scheme-based audio pipelines.

Planned enhancements include:
- **Replication:** Multi-node clustering for distributed storage and redundancy
- **Compaction:** Offline SSTable compaction to optimize disk usage
- **TTL Retention Policies:** Cleanup of expired audio data
- **Compression:** Reduced on-disk footprint for archived segments
- **DSP Functions:** Built-in signal processing functions


