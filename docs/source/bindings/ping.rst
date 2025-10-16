.. _SCM_PING:

PING
====

**Description:**
Checks the connectivity and responsiveness of the server.

Output
------

+------------+--------------------------------------------+
| Type       | Description                                |
+============+============================================+
| ``String`` | Always returns the string ``'PONG'``.      |
+------------+--------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (ping)

Using ``EVAL``:

.. code-block:: none

    EVAL '(ping)'
