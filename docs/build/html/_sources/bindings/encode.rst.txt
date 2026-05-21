.. _SCM_ENCODE:

encode
======

Encodes PCM buffer to MP3, Opus, or WAV.

**note**: The ``data`` argument must be the result of a RACS command binding. Do not pass custom input using ``#s32()``.

Arguments
---------

+-------------+------------+-----------------------------------------------+
| Name        | Type       | Description                                   |
+=============+============+===============================================+
| data        | ``#s32``   | PCM buffer.                                   |
+-------------+------------+-----------------------------------------------+
| mime_type   | ``String`` | MIME type of the encoded audio output.        |
+-------------+------------+-----------------------------------------------+


Supported MIME types
^^^^^^^^^^^^^^^^^^^^

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
    (encode (range "vocals" 0.0 30.0) "audio/wav")

Using ``EVAL``:

.. code-block:: none

   EVAL '(encode (range \"vocals\" 0.0 30.0) \"audio/wav\")'
