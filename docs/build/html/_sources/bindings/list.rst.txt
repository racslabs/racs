.. _SCM_LIST:

list
====

Lists all the stream ids matching wildcard pattern.

Arguments
---------

+-------------+------------+---------------------------------------------------------------+
| Name        | Type       | Description                                                   |
+=============+============+===============================================================+
| pattern     | ``String`` | Wildcard pattern to match against.                            |
+-------------+------------+---------------------------------------------------------------+

Output
------

+------------+-----------------------------------------------------+
| Type       | Description                                         |
+============+=====================================================+
| ``List``   | List of stream ids that match the wildcard pattern. |
+------------+-----------------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (list "*")

Using ``EVAL``:

.. code-block:: none

    EVAL '(list \"*\")'