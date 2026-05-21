.. _SCM_PAN:

pan
===

Positions audio within the stereo field.
Values less than zero pan to the left. Values greater then zero pan to the right.

**note**: The ``data`` argument must be the result of a RACS command binding. Do not pass custom input using ``#s32()``.

Arguments
---------

+-----------+------------+------------------------------------------------------+
| Name      | Type       | Description                                          |
+===========+============+======================================================+
| data      | ``#s32``   | PCM buffer.                                          |
+-----------+------------+------------------------------------------------------+
| pan       | ``Float``  | Pan level.                                           |
+-----------+------------+------------------------------------------------------+


Output
------

+----------+-------------------------------------------------------------+
| Type     | Description                                                 |
+==========+==========+==================================================+
| ``#s32`` | PCM buffer with pan applied.                                |
+----------+-------------------------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (pan (range "vocals" 0.0 30.0) -0.1)

Using ``EVAL``:

.. code-block:: none

    EVAL '(pan (range \"vocals\" 0.0 30.0) -0.1)'
