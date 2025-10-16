Scheme Bindings
===============

**Description:**
RACS includes an embedded Guile Scheme interpreter, allowing Scheme expressions to interact directly with the server.
This is done using the :ref:`EVAL` command.

.. note::

   For safety, Scheme evaluation is sandboxed. Only the operators ``+``, ``-``, ``*``, ``list`` and the RACS command bindings are allowed in the sandbox. This prevents arbitrary code execution.

Supported Command Bindings
--------------------------

The following RACS commands are available as Scheme functions:

:ref:`extract <SCM_EXTRACT>`

:ref:`create <SCM_CREATE>`

:ref:`info <SCM_INFO>`

:ref:`format <SCM_FORMAT>`

:ref:`ls <SCM_LS>`

:ref:`ping <SCM_PING>`

:ref:`open <SCM_OPEN>`

:ref:`close <SCM_CLOSE>`

:ref:`shutdown <SCM_SHUTDOWN>`


Supported Scheme Data Types
---------------------------

The result of a Scheme expression is automatically mapped to the corresponding RACS data type as shown below:

+-------------------+-------------------------------------------------------------+
| Scheme type       | RACS type                                                   |
+===================+=============================================================+
| Integer           | ``int``                                                     |
+-------------------+-------------------------------------------------------------+
| Float             | ``float``                                                   |
+-------------------+-------------------------------------------------------------+
| Boolean           | ``bool``                                                    |
+-------------------+-------------------------------------------------------------+
| Null / nil        | ``null``                                                    |
+-------------------+-------------------------------------------------------------+
| Pair / list       | ``list``                                                    |
+-------------------+-------------------------------------------------------------+
| String            | ``string``                                                  |
+-------------------+-------------------------------------------------------------+
| Typed arrays      | ``s8v``, ``u8v``, ``s16v``, ``u16v``, ``s32v``, ``u32v``,   |
|                   | ``f32v``, ``c64v``                                          |
+-------------------+-------------------------------------------------------------+
