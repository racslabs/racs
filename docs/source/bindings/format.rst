.. _SCM_FORMAT:

FORMAT
======

**Description:**
Formats raw PCM data (interleaved by channel) into MP3, Opus, or WAV.

.. note::

   ``FORMAT`` does not perform resampling. The sample rate, channel count,
   and bit depth must match the input exactly.

Arguments
---------

+-------------+---------------+-----------------------------------------------+
| Name        | Type          | Description                                   |
+=============+===============+===============================================+
| data        | ``#s32``      | PCM data interleaved by channel.              |
+-------------+---------------+-----------------------------------------------+
| mime_type   | ``String``    | MIME type of the audio output.                |
+-------------+---------------+-----------------------------------------------+
| sample_rate | ``Integer``   | Output sample rate (Hz).                      |
+-------------+---------------+-----------------------------------------------+
| channels    | ``Integer``   | Number of output channels.                    |
+-------------+---------------+-----------------------------------------------+
| bit_depth   | ``Integer``   | Bits per sample in output.                    |
+-------------+---------------+-----------------------------------------------+

**Supported MIME types**

+-----------------+---------------------------+
| MIME type       | Description               |
+=================+===========================+
| ``audio/wav``   | WAV format                |
+-----------------+---------------------------+
| ``audio/mpeg``  | MPEG / MP3 format         |
+-----------------+---------------------------+
| ``audio/mp3``   | MP3 format (alias)        |
+-----------------+---------------------------+
| ``audio/ogg``   | Ogg/Opus format           |
+-----------------+---------------------------+

Output
------

+--------------+----------------------------------------------------+
| Type         | Description                                        |
+==============+====================================================+
| ``#u8``      | The encoded audio data.                            |
+--------------+----------------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (format #s32(1 2 3 4) "audio/wav" 44100 2 16)

Using ``EVAL``:

.. code-block:: none

   EVAL '(format #s32(1 2 3 4) \"audio/wav\" 44100 2 16)'
