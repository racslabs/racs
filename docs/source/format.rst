.. _FORMAT:

FORMAT
======

**Description:**
Formats raw PCM data interleaved by channel as MP3, Opus, or WAV.

.. note::

   ``FORMAT`` does not perform resampling. The sample rate, channel count,
   and bit depth must accurately reflect the input.

Arguments
---------

+-------------+------------+---------------------------------------------------------------+
| Name        | Type       | Description                                                   |
+=============+============+===============================================================+
| mime_type   | ``string`` | MIME type of the audio output. Supported values:              |
|             |            | ``audio/wav``, ``audio/mpeg``, ``audio/mp3``, ``audio/ogg``.  |
+-------------+------------+---------------------------------------------------------------+
| sample_rate | ``int``    | Output sample rate (Hz).                                      |
+-------------+------------+---------------------------------------------------------------+
| channels    | ``int``    | Number of output channels.                                    |
+-------------+------------+---------------------------------------------------------------+
| bit_depth   | ``int``    | Bits per sample.                                              |
+-------------+------------+---------------------------------------------------------------+

Dependencies
------------
- :ref:`EXTRACT`

Output
------

+--------+---------+--------------------------------------------+
| Name   | Type    | Description                                |
+========+=========+============================================+
| data   | ``uv8`` | Encoded audio data in the specified format.|
+--------+---------+--------------------------------------------+

Example
-------

.. code-block:: none

   FORMAT 'audio/mp3' 44100 2 16

