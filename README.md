<p align="center">
    <img width="350px" src="logo.svg"  alt=""/>
</p>

## What is RACS?

RACS (Remote Audio Caching Server) is a distributed, persistent audio cache optimized for time-indexed range queries. It uses a hybrid memory-disk storage model for low-latency ingestion and retrieval, enabling fine-grained access to arbitrary time ranges within large audio archives while minimizing disk I/O.

## Features

- **Format Conversion** — Transcode audio to PCM, WAV, Opus and MP3.
- **Time-Range Queries** — Retrieve precise audio segments from large archives.
- **Caching & Persistence** — Hybrid memory–disk storage with LRU caching.
- **Programmability** — Embedded Guile Scheme interpreter for scriptable server interaction.
- **Replication** — Basic master-slave replication.
- **WAL** — Write-ahead log for crash recovery.

## ⚠️ MVP Stage

RACS is currently in the Minimum Viable Product (MVP) stage. It is under active testing. New features and enhancements will be added in future releases. Feedback is welcome!

## Licensing

RACS is distributed under the RACS Source Available License (RACS-SAL-1.0).  
It is free for personal, educational, and non-commercial use. Commercial use requires a paid license. For licensing inquiries, contact sales@racslabs.com.

## Generating Documentation

Install dependencies and build HTML docs with:

```sh
pip install sphinx Pygments sphinx-copybutton
cd docs && make html
```

Open `docs/build/html/index.html` in a browser to view the documentation.

## Client Libraries

RACS provides client libraries for multiple languages:

- [**(Python)** py-racs](https://github.com/racslabs/py-racs)
- [**(Rust)** rust-racs](https://github.com/racslabs/rust-racs)
- [**(Java)** java-racs](https://github.com/racslabs/java-racs)
