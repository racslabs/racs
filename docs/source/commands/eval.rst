.. _EVAL:

EVAL
====

**Description:**
Evaluates a Scheme (SCM) expression using the Guile Scheme interpreter.

Arguments
---------

+-------------+------------+---------------------------------------------------------------+
| Name        | Type       | Description                                                   |
+=============+============+===============================================================+
| scm_expr    | ``string`` | Scheme expression to be evaluated.                            |
+-------------+------------+---------------------------------------------------------------+

Dependencies
------------
- ``NONE``

Output
------

+----------+--------------------------------------------+
| Type     | Description                                |
+==========+============================================+
| ``any``  | Result of the evaluated Scheme expression. |
+----------+--------------------------------------------+

Example
-------

.. code-block:: none

   EVAL '(+ 1 2 3)'
