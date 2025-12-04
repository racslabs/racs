.. _SCM_SHUTDOWN:

SHUTDOWN
========

**Description:**
Stops the RACS server.

.. note::

   Use ``SHUTDOWN`` to ensure all buffers are flushed before termination. This prevents data loss.

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (shutdown)

Using ``EVAL``:

.. code-block:: none

   `EVAL '(shutdown)'
