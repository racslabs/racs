.. _SCM_PAD:

pad
===

Adds silence to the beginning and end of a PCM buffer.

**note**: The ``data`` argument must be the result of a RACS command binding. Do not pass custom input using ``#s32()``.

Arguments
---------

+-----------+------------+---------------------------------------------------------+
| Name      | Type       | Description                                             |
+===========+============+=========================================================+
| data      | ``#s32``   | PCM buffer.                                             |
+-----------+------------+---------------------------------------------------------+
| left      | ``Float``  | Duration (in seconds) of silence added at the beginning.|
+-----------+------------+---------------------------------------------------------+
| right     | ``Float``  | Duration (in seconds) of silence added at the end.      |
+-----------+------------+---------------------------------------------------------+

Output
------

+----------+-------------------------------------------------------------+
| Type     | Description                                                 |
+==========+==========+==================================================+
| ``#s32`` | Padded PCM buffer.                                          |
+----------+-------------------------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (pad (range "vocals" 0.0 30.0) 10.0 15.0)

Using ``EVAL``:

.. code-block:: none

    EVAL '(pad (range \"vocals\" 0.0 30.0) 10.0 15.0)'