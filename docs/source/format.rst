.. _FORMAT:

FORMAT
======

**Description:**
Formats raw PCM data (interleaved by channel) into MP3, Opus, or WAV.

.. note::

   ``FORMAT`` does not perform resampling. The sample rate, channel count,
   and bit depth must match the input exactly.

Arguments
---------

+-------------+------------+-----------------------------------------------+
| Name        | Type       | Description                                   |
+=============+============+===============================================+
| mime_type   | ``string`` | MIME type of the audio output.                |
+-------------+------------+-----------------------------------------------+
| sample_rate | ``int``    | Output sample rate (Hz).                      |
+-------------+------------+-----------------------------------------------+
| channels    | ``int``    | Number of output channels.                    |
+-------------+------------+-----------------------------------------------+
| bit_depth   | ``int``    | Bits per sample in output.                    |
+-------------+------------+-----------------------------------------------+

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

Dependencies
------------
- :ref:`EXTRACT`

Output
------

+---------+----------------------------------------------------+
| Type    | Description                                        |
+=========+====================================================+
| ``uv8`` | 1-D unsigned byte vector containing encoded audio. |
+---------+----------------------------------------------------+

Example
-------

.. code-block:: none

   FORMAT 'audio/mp3' 44100 2 16
