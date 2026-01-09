.. _SCM_FADE:

fade
====

Applies a linear fade-in at the start of the PCM buffer and a linear fade-out at the end.

**note**: The ``data`` argument must be the result of a RACS command binding. Do not pass custom input using ``#s32()``.

Arguments
---------

+-----------+------------+------------------------------------------------------+
| Name      | Type       | Description                                          |
+===========+============+======================================================+
| data      | ``#s32``   | PCM buffer.                                          |
+-----------+------------+------------------------------------------------------+
| left      | ``Float``  | Duration (in seconds) of fade-in at the beginning.   |
+-----------+------------+------------------------------------------------------+
| right     | ``Float``  | Duration (in seconds) of fade-out at the end.        |
+-----------+------------+------------------------------------------------------+

Output
------

+----------+-------------------------------------------------------------+
| Type     | Description                                                 |
+==========+==========+==================================================+
| ``#s32`` | PCM buffer with fade applied.                               |
+----------+-------------------------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (fade (range "vocals" 0.0 30.0) 5.0 10.0)

Using ``EVAL``:

.. code-block:: none

    EVAL '(fade (range \"vocals\" 0.0 30.0) 5.0 10.0)'
