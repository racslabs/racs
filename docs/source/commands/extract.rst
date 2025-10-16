.. _EXTRACT:

EXTRACT
=======

**Description:**
Extracts raw PCM data interleaved by channel for the given stream id and timestamp range.

Arguments
---------

+-----------+------------+------------------------------------------------------+
| Name      | Type       | Description                                          |
+===========+============+======================================================+
| stream_id | ``string`` | ASCII ID of the audio stream.                        |
+-----------+------------+------------------------------------------------------+
| from      | ``time``   | Start timestamp in the stream, RFC 3339 format:      |
|           |            | ``YYYY-MM-DDTHH:MM:SS:MSZ`` (UTC, no offset).        |
+-----------+------------+------------------------------------------------------+
| to        | ``time``   | End timestamp in the stream, RFC 3339 format:        |
|           |            | ``YYYY-MM-DDTHH:MM:SS:MSZ`` (UTC, no offset).        |
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

.. code-block:: none

   EXTRACT 'chopin' 2023-12-25T22:30:45.123Z 2024-05-27T02:56:16.123Z
