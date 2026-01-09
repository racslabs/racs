.. _SCM_GAIN:

gain
====

Multiplies each sample in the PCM buffer by the specified gain level.
Values above one boost the signal. Values below one attenuate it.

**note**: The ``data`` argument must be the result of a RACS command binding. Do not pass custom input using ``#s32()``.

Arguments
---------

+-------------+------------+---------------------------------------------------------------+
| Name        | Type       | Description                                                   |
+=============+============+===============================================================+
| data        | ``#s32``   | PCM buffer.                                                   |
+-------------+------------+---------------------------------------------------------------+
| gain        | ``Float``  | Gain level.                                                   |
+-------------+------------+---------------------------------------------------------------+

Output
------

+----------+-------------------------------------------------------------+
| Type     | Description                                                 |
+==========+==========+==================================================+
| ``#s32`` | PCM buffer with gain applied.                               |
+----------+-------------------------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (gain (range "vocals" 0.0 30.0) 0.8)

Using ``EVAL``:

.. code-block:: none

   EVAL '(gain (range \"vocals\" 0.0 30.0) 0.8)'
