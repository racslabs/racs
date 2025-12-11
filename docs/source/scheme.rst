Scheme Bindings
===============

**Description:**
RACS includes an embedded Guile Scheme interpreter, allowing direct interaction with the server using Scheme expressions.
This is done using the :ref:`EVAL` command.

.. note::

    For security, Scheme evaluation is sandboxed. Only the operators ``+``, ``-``, ``*``, ``list`` and the RACS command bindings are allowed in the sandbox. This prevents arbitrary code execution.

Supported Command Bindings
--------------------------

The following RACS commands are available as Scheme functions:

:ref:`extract <SCM_EXTRACT>`

:ref:`info <SCM_INFO>`

:ref:`format <SCM_FORMAT>`

:ref:`search <SCM_SEARCH>`

:ref:`ping <SCM_PING>`


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

If the final result of the Scheme expression cannot be serialized to one of the above RACS data types, an error message will be returned.
