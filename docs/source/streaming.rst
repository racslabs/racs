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
| channels   | Number of audio channels                                    | 2          | 31     | Little    |
+------------+-------------------------------------------------------------+------------+--------+-----------+
| sample_rate| Audio sample rate (samples per second)                      | 4          | 33     | Little    |
+------------+-------------------------------------------------------------+------------+--------+-----------+
| bit_depth  | Number of bits per sample                                   | 2          | 37     | Little    |
+------------+-------------------------------------------------------------+------------+--------+-----------+
| block_size | Size of PCM encoded block in bytes                          | 2          | 39     | Little    |
|            | Max block size is 2^16 bytes (64KB)                         |            |        |           |
+------------+-------------------------------------------------------------+------------+--------+-----------+
| flags      | Compression flag (0 = uncompressed, 1 = compressed)         | 1          | 41     | N/A       |
+------------+-------------------------------------------------------------+------------+--------+-----------+
| pcm_block  | Block containing the raw PCM samples interleaved by channel | block_size | 42     | Little    |
+------------+-------------------------------------------------------------+------------+--------+-----------+

.. note::

    Only 16-bit and 24-bit PCM samples are supported.
    Compression is not yet implemented. Set flags to 0.
