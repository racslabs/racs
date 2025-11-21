<p align="center">
    <img width="350px" src="logo.svg"  alt=""/>
</p>

![CircleCI](https://img.shields.io/circleci/build/gh/CorbinMoon/racs)


## What is RACS?

**RACS (Remote Audio Caching Server)** is an append-only, persistent audio caching server with built-in range queries, format conversion, and low-latency streaming.
Unlike blob stores and traditional databases, **RACS** is purpose-built for storing, querying, and streaming audio data at scale.

### Key Features

- **Low-Latency Streaming**: Real-time delivery of audio directly from memory or disk. Ideal for edge caches, Content Delivery Networks (CDN), and application servers.
- **Format Conversion**: On-the-fly transcoding to MP3, Opus, and WAV formats to adapt to client or bandwidth requirements.
- **Time-Range Queries**: Retrieve precise audio segments by timestamp for efficient slicing, analysis, or playback.
- **Metadata Management**: Store and query audio-specific metadata such as sample rate, bit depth, and channels.
- **Caching & Persistence**: Hybrid memory–disk storage with LRU caching for fast access and durable persistence.
- **Programmability**: Embedded Guile Scheme interpreter for direct, scriptable interaction with the server. 

### Storage Engine

**RACS** uses a multi-tiered architecture inspired by LSM (Log-Structured Merge) trees:

1. **Memtables**:
Incoming audio blocks are written in memory and batched for efficient disk flushes.
2. **SSTables (Time-Partitioned)**:
When memtables reach capacity, they are flushed as immutable time-partitioned files on disk. This layout enables fast range queries by timestamp.
3. **LRU Cache**:
Frequently accessed SSTables are cached in memory, minimizing disk I/O and improving response times.

This architecture ensures a balance between high write performance, query efficiency, and data durability.

## Why Use RACS?

Audio deserves more than generic storage.

RACS is built for systems where audio is dynamic, time-indexed, and performance-critical.
It replaces complex pipelines and slow object stores with a single, purpose-built engine for fast, precise, and reliable audio access.

When you need to treat audio as data — not files — RACS is the right tool.

## ⚠️ MVP Stage

**RACS** is currently in the **MVP (Minimum Viable Product)** stage. All the key features mentioned above are fully functional.

Planned enhancements include:
- **Replication:** Multi-node clustering for distributed storage and redundancy
- **Compaction:** Offline SSTable compaction to optimize disk usage
- **TTL Retention Policies:** Cleanup of expired audio data during compaction
- **Compression:** Reduced on-disk footprint for archived segments and bandwidth usage during ingestion
- **DSP Module:** Extend functionality to include SIMD optimized signal processing functions


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

You can now connect to the server on `localhost:6381`.

### Other Build Options

- [Build From Source](docs/source/build-from-source.rst)

### Documentation Generation

To generate the docs, run the following commands:

```sh
    pip install -U sphinx
```

```sh
    pip install piccolo-theme
```

```sh
    cd docs && make html
```

Open the `index.html` file under `docs/build/html` in the browser.

> [!NOTE]  
> python 3+ is required for doc generation.

### Client Libraries

RACS provides client libraries for multiple languages:

- [**(Python)** py-racs](https://github.com/racslabs/py-racs) — fully usable
- [**(Java)** java-racs](https://github.com/racslabs/java-racs) — under development
- [**(Rust)** rust-racs](https://github.com/racslabs/rust-racs) — fully usable
- [**(C)** racs-client](https://github.com/racslabs/racs-client) — under development
