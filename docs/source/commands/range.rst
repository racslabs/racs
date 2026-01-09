.. _RANGE:

RANGE
=====

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
| duration  | ``float``  | Duration of audio segment to extract (in seconds).   |
+-----------+------------+------------------------------------------------------+

Input
-----

+----------+--------------------------------------------+
| Type     | Description                                |
+==========+============================================+
| ``null`` | N/A                                        |
+----------+--------------------------------------------+

Output
------

+----------+-------------------------------------------------------------+
| Type     | Description                                                 |
+==========+==========+==================================================+
| ``s32v`` | PCM buffer containing samples interleaved by channel.       |
|          | Both 16-bit and 24-bit samples are stored in ``s32v``.      |
+----------+-------------------------------------------------------------+

Example
-------

.. code-block:: bash

   RANGE 'vocals' 0.0 30.0
