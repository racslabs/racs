Streaming
=========

**RACS Streaming Protocol** (RSP) is used to efficiently transfer audio data over TCP/IP.

The client takes raw PCM samples interleaved by channel and chunks it into frames with the below format:

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

.. note::

    Only 16-bit and 24-bit PCM samples are supported.
    Compression is not yet implemented. Set flags to 0.

Timestamp Generation
--------------------

When a new stream is created, a reference timestamp is generated when the first
frame is appended to the stream.
The timestamp for each subsequent frame is computed as an offset from this
reference timestamp using the following formula:

.. math::

    t = \left(
        \frac{\text{size}}
        {\text{channels} \times \text{sample_rate} \times \left(\frac{\text{bit_depth}}{8}\right)}
        \right) \times 1000 \;+\; \text{ref}

Where:

- ``size`` is the current size of the stream in bytes.
- ``channels`` is the number of channels.
- ``sample_rate`` is the sample rate of the stream.
- ``bit_depth`` is the number of bits per sample.
- ``ref`` is the reference timestamp (milliseconds UTC) established when the first frame is appended.

