.. _SCM_RANGE:

range
=====

Extracts raw PCM data interleaved by channel for the given stream id and range.

Arguments
---------

+-----------+------------+------------------------------------------------------+
| Name      | Type       | Description                                          |
+===========+============+======================================================+
| stream_id | ``String`` | ASCII ID of the audio stream.                        |
+-----------+------------+------------------------------------------------------+
| start     | ``Float``  | Start time (in seconds) to extract from the stream.  |
+-----------+------------+------------------------------------------------------+
| duration  | ``Float``  | Duration of audio segment to extract (in seconds).   |
+-----------+------------+------------------------------------------------------+

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

.. code-block:: scheme

    ;; SCM expression
    (range "vocals" 0.0 30.0)

Using ``EVAL``:

.. code-block:: none

    EVAL '(range \"vocals\" 0.0 30.0)'
