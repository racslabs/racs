.. _SCM_MIX:

mix
===

Combines two single-channel PCM buffers into one stereo PCM buffer.

**note**: The ``data`` arguments must be the result of a RACS command binding. Do not pass custom input using ``#s32()``.

Arguments
---------

+-------------+------------+-----------------------------------------------+
| Name        | Type       | Description                                   |
+=============+============+===============================================+
| data0       | ``#s32``   | First PCM buffer. Mono only.                  |
+-------------+------------+-----------------------------------------------+
| data1       | ``#s32``   | Second PCM buffer. Mono only.                 |
+-------------+------------+-----------------------------------------------+

Output
------

+--------------+----------------------------------------------------+
| Type         | Description                                        |
+==============+====================================================+
| ``#s32``     | Stereo PCM buffer.                                 |
+--------------+----------------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (merge (range "vocals" 0.0 30.0) (range "instrumentals" 0.0 30.0))

Using ``EVAL``:

.. code-block:: none

   EVAL '(merge (range \"vocals\" 0.0 30.0) (range \"instrumentals\" 0.0 30.0))'
