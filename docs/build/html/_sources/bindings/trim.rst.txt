.. _SCM_TRIM:

trim
====

Removes audio from the left and/or right sides of a PCM buffer.

**note**: The ``data`` argument must be the result of a RACS command binding. Do not pass custom input using ``#s32()``.

Arguments
---------

+-----------+------------+---------------------------------------------------------+
| Name      | Type       | Description                                             |
+===========+============+=========================================================+
| data      | ``#s32``   | PCM buffer.                                             |
+-----------+------------+---------------------------------------------------------+
| left      | ``Float``  | Duration (in seconds) of audio to remove from the left. |
+-----------+------------+---------------------------------------------------------+
| right     | ``Float``  | Duration (in seconds) of audio to remove from the right.|
+-----------+------------+---------------------------------------------------------+


Output
------

+----------+-------------------------------------------------------------+
| Type     | Description                                                 |
+==========+==========+==================================================+
| ``#s32`` | Trimmed PCM buffer.                                         |
+----------+-------------------------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (trim (range "vocals" 0.0 90.0) 30.0 30.0)

Using ``EVAL``:

.. code-block:: none

   EVAL '(trim (range \"vocals\" 0.0 90.0) 30.0 30.0)'