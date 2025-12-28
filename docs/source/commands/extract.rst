.. _EXTRACT:

EXTRACT
=======

**Description:**
Extracts raw PCM data interleaved by channel for the given stream id and range.

Arguments
---------

+-----------+------------+------------------------------------------------------+
| Name      | Type       | Description                                          |
+===========+============+======================================================+
| stream_id | ``string`` | ASCII ID of the audio stream.                        |
+-----------+------------+------------------------------------------------------+
| start     | ``float``  | Start time (in seconds) to extract from the stream.  |
+-----------+------------+------------------------------------------------------+
| duration  | ``float``  | Druation of audio segment to extract.                |
+-----------+------------+------------------------------------------------------+

Dependencies
------------
- ``NONE``

Output
------

+----------+-------------------------------------------------------------+
| Type     | Description                                                 |
+==========+==========+==================================================+
| ``s32v`` | Raw PCM samples interleaved by channel.                     |
|          | Both 16-bit and 24-bit samples are stored in ``s32v``.      |
+----------+-------------------------------------------------------------+

Example
-------

.. code-block:: bash

   EXTRACT 'chopin' 0.0 13.5
