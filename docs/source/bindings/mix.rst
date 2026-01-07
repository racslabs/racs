.. _SCM_MIX:

mix
===

Combines two PCM buffers but adding the samples together.

**note**: The ``data`` arguments must be the result of a RACS command binding. Do not pass custom input using ``#s32()``.

Arguments
---------

+-------------+------------+-----------------------------------------------+
| Name        | Type       | Description                                   |
+=============+============+===============================================+
| data0       | ``#s32``   | First PCM buffer.                             |
+-------------+------------+-----------------------------------------------+
| data1       | ``#s32``   | Second PCM buffer.                            |
+-------------+------------+-----------------------------------------------+


Output
------

+--------------+----------------------------------------------------+
| Type         | Description                                        |
+==============+====================================================+
| ``#s32``     | Combined PCM buffer.                               |
+--------------+----------------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (mix (range "vocals" 0.0 30.0) (range "instrumentals" 0.0 30.0))

Using ``EVAL``:

.. code-block:: none

   EVAL '(mix (range \"vocals\" 0.0 30.0) (range \"instrumentals\" 0.0 30.0))'
