Streaming
=========

RACS Streaming Protocol
-----------------------

**RACS Streaming Protocol** (RSP) is used to efficiently transfer audio data over TCP/IP.

Clients send PCM samples interleaved by channel in frames, batching multiple frames per TCP transmission. Only 16-bit and 24-bit PCM samples are supported.

Frame Format
^^^^^^^^^^^^

+------------+-------------------------------------------------------------+------------+--------+-----------+
| Value      | Description                                                 | Bytes      | Offset | Byte Order|
+============+=============================================================+============+========+===========+
| chunk_id   | The ASCII string ``"rsp"``                                  | 3          | 0      | N/A       |
+------------+-------------------------------------------------------------+------------+--------+-----------+
| session_id | 128-bit unique session id (UUID)                            | 16         | 3      | Little    |
+------------+-------------------------------------------------------------+------------+--------+-----------+
| hash       | 64-bit hash of the stream ID                                | 8          | 19     | Little    |
+------------+-------------------------------------------------------------+------------+--------+-----------+
| checksum   | 32-bit CRC32C checksum for error detection                  | 4          | 27     | Little    |
+------------+-------------------------------------------------------------+------------+--------+-----------+
| block_size | Size of PCM encoded block in bytes                          | 2          | 31     | Little    |
|            | Max block size is 2^16 bytes (64KB)                         |            |        |           |
+------------+-------------------------------------------------------------+------------+--------+-----------+
| flags      | Compression flag (0 = uncompressed, 1 = compressed)         | 1          | 33     | N/A       |
+------------+-------------------------------------------------------------+------------+--------+-----------+
| pcm_block  | Block containing the raw PCM samples interleaved by channel | block_size | 34     | Little    |
+------------+-------------------------------------------------------------+------------+--------+-----------+

Block Sizes
-----------

The minimum uncompressed block size is 1024 bytes (1 KB). All uncompressed block sizes must be a power of two. Smaller block sizes result in slower ingestion but provide more precise time-range queries. Larger block sizes allow faster ingestion at the cost of lower time-range precision.

Timestamp Generation
--------------------

When a new stream is created, a reference timestamp is generated when the first frame is appended. The timestamp for each subsequent frame is calculated as an offset from this reference based on the number of PCM samples per block, the sample rate, and the number of channels. This ensures accurate alignment for time-range queries.

