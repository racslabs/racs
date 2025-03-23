Types
=====

Basic Types
-----------

+---------------+------------------------------------------------------------+
| ``null``      | Represents a missing or unknown value.                     |
+---------------+------------------------------------------------------------+
| ``bool``      | Represents a truth value. Is either ``true`` or ``false``. |
+---------------+------------------------------------------------------------+
| ``int``       | 64-bit signed integer.                                     |
+---------------+------------------------------------------------------------+
| ``float``     | IEEE-754 64-bit floating-point number.                     |
+---------------+------------------------------------------------------------+

.. important::

   Basic Types are not nullable.

String Types
------------

Type ``string`` represents a sequence of characters. All strings are expected to be UTF-8 encoded.

Both single and double quotes can be used to a string.

.. important::

   ``string`` is not nullable.


Time Types
----------

Type ``time`` represents a single point in time with millisecond precision.

``time`` types are represented in RFC3339 format. The supported formats are:

- ``YYYY-MM-DD``
- ``YYYY-MM-DDT00:00:000Z``

