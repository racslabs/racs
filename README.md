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
- **Metadata Management:** Manage audio specific metadata attributes
- **Embedded Scheme Interpreter:** Run programmable Scheme-based audio pipelines


## ⚙️ Storage Engine Mechanics

**RACS** uses a multi-tiered architecture inspired by LSM (Log-Structured Merge) trees:

1. **Memtables**

    Incoming audio blocks are written in memory and batched for efficient disk flushes.
2. **SSTables (Time-Partitioned)**

    When memtables reach capacity, they are flushed as immutable time-partitioned files on disk. This layout enables fast range queries by timestamp.
3. **LRU Cache**

    Frequently accessed SSTables are cached in memory, minimizing disk I/O and improving response times.

This architecture ensures a balance between high write performance, query efficiency, and data durability.
