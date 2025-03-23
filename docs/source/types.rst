Types
=====

Basic Types
-----------

+---------------+------------------------------------------------------------+
| ``bool``      | Represents a truth value. Is either ``true`` or ``false``. |
+---------------+------------------------------------------------------------+
| ``int``       | 64-bit signed integer.                                     |
+---------------+------------------------------------------------------------+
| ``float``     | IEEE-754 64-bit floating-point number.                     |
+---------------+------------------------------------------------------------+

Null Types:
-----------

Type ``null`` represents a missing or unknown value.

String Types
------------

Type ``string`` represents a sequence of characters. All strings are expected to be UTF-8 encoded.

Both single and double quotes can be used to a string.


Time Types
----------

Type ``time`` represents a single point in time with millisecond precision.

``time`` types are represented in RFC3339 format. The supported formats include:

- ``YYYY-MM-DD``
- ``YYYY-MM-DDT00:00:000Z``


List Types
----------

Type ``list`` is an ordered collection of values.

A list may contain mixed types but they must all be basic types or strings.

For example: ``("1" 2 false)`` is valid, but ``((1 2) false)`` is not.


Numeric Vector Types
--------------------

+---------------+--------------------------------------------------------------------------------------------------------------------------+
| ``i8v``       | Represents a 1-D vector of signed bytes. Binary data is returned as ``i8v`` by default.                                  |
+---------------+--------------------------------------------------------------------------------------------------------------------------+
| ``u8v``       | Represents a 1-D vector of unsigned bytes.                                                                               |
+---------------+--------------------------------------------------------------------------------------------------------------------------+
| ``i16v``      | Represents a 1-D vector of signed 16-bit integers.                                                                       |
+---------------+--------------------------------------------------------------------------------------------------------------------------+
| ``u16v``      | Represents a 1-D vector of unsigned 16-bit integers.                                                                     |
+---------------+--------------------------------------------------------------------------------------------------------------------------+
| ``i32v``      | Represents a 1-D vector of signed 32-bit integers. PCM data is returned as ``i32v`` by default.                          |
+---------------+--------------------------------------------------------------------------------------------------------------------------+
| ``u32v``      | Represents a 1-D vector of unsigned 32-bit integers.                                                                     |
+---------------+--------------------------------------------------------------------------------------------------------------------------+
| ``f32v``      | Represents a 1-D vector of IEEE-754 32-bit floating-point numbers.                                                       |
+---------------+--------------------------------------------------------------------------------------------------------------------------+
| ``c64v``      | Represents a 1-D vector of complex numbers where the real and imaginary parts are IEEE-754 32-bit floating-point numbers.|
+---------------+--------------------------------------------------------------------------------------------------------------------------+

``f64v`` and ``c128v`` are not yet supported, but will be in the near future.
