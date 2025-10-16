.. _SCM_EXTRACT:

EXTRACT
=======

**Description:**
Extracts raw PCM data interleaved by channel for the given stream id and timestamp range.

Arguments
---------

+-----------+------------+------------------------------------------------------+
| Name      | Type       | Description                                          |
+===========+============+======================================================+
| stream_id | ``String`` | ASCII ID of the audio stream.                        |
+-----------+------------+------------------------------------------------------+
| from      | ``String`` | Start timestamp in the stream, RFC 3339 format:      |
|           |            | ``YYYY-MM-DDTHH:MM:SS:MSZ`` (UTC, no offset).        |
+-----------+------------+------------------------------------------------------+
| to        | ``String`` | End timestamp in the stream, RFC 3339 format:        |
|           |            | ``YYYY-MM-DDTHH:MM:SS:MSZ`` (UTC, no offset).        |
+-----------+------------+------------------------------------------------------+

Output
------

+---------------+-------------------------------------------------------------+
| Type          | Description                                                 |
+===============+==========+==================================================+
| ``#s32``      | Raw PCM samples interleaved by channel.                     |
|               | Both 16-bit and 24-bit samples are stored in ``#s32``.      |
+---------------+-------------------------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (extract "chopin" "2023-12-25T22:30:45.123Z" "2024-05-27T02:56:16.123Z")

Using ``EVAL``:

.. code-block:: none

    EVAL '(extract \"chopin\" \"2023-12-25T22:30:45.123Z\" \"2024-05-27T02:56:16.123Z\")'
