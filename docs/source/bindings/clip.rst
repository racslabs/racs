.. _SCM_CLIP:

CLIP
====

Enforces a hard minimum and maximum amplitude for sample values outside the threshold.

**note**: The ``data`` argument must be the result of a RACS command binding. Do not pass custom input using ``#s32()``.

Arguments
---------

+-----------+------------+------------------------------------------------------+
| Name      | Type       | Description                                          |
+===========+============+======================================================+
| data      | ``#s32``   | PCM buffer.                                          |
+-----------+------------+------------------------------------------------------+
| min       |``Integer`` | Minimum amplitude.                                   |
+-----------+------------+------------------------------------------------------+
| max       |``Integer`` | Maximum amplitude.                                   |
+-----------+------------+------------------------------------------------------+


Output
------

+----------+-------------------------------------------------------------+
| Type     | Description                                                 |
+==========+==========+==================================================+
| ``#s32`` | Clipped PCM buffer.                                         |
+----------+-------------------------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (clip (range "vocals" 0.0 30.0) -30000 30000)

Using ``EVAL``:

.. code-block:: none

    EVAL '(clip (range \"vocals\" 0.0 30.0) -30000 30000)'
