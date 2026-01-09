.. _SCM_SPLIT:

split
=====

Extracts a single channel from a stereo PCM buffer.

**note**: The ``data`` argument must be the result of a RACS command binding. Do not pass custom input using ``#s32()``.

Arguments
---------

+-----------+------------+------------------------------------------------------+
| Name      | Type       | Description                                          |
+===========+============+======================================================+
| data      | ``#s32``   | PCM buffer.                                          |
+-----------+------------+------------------------------------------------------+
| channel   | ``int``    | Channel to extract.                                  |
+-----------+------------+------------------------------------------------------+


Output
------

+----------+-------------------------------------------------------------+
| Type     | Description                                                 |
+==========+==========+==================================================+
| ``#s32`` | Extracted single-channel PCM buffer.                        |
+----------+-------------------------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (split (range "vocals" 0.0 30.0) 0)

Using ``EVAL``:

.. code-block:: none

    EVAL '(split (range \"vocals\" 0.0 30.0) 0)'