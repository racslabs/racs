.. _INFO:

INFO
====

**Description:**
Retrieves metadata attributes of an audio stream.

Arguments
---------

+-------------+------------+---------------------------------------------------------------+
| Name        | Type       | Description                                                   |
+=============+============+===============================================================+
| stream_id   | ``string`` | ASCII ID of the audio stream.                                 |
+-------------+------------+---------------------------------------------------------------+
| attribute   | ``string`` | Name of the metadata attribute to retrieve.                   |
+-------------+------------+---------------------------------------------------------------+

**Supported Attributes**

+-----------------+--------------------------------------------+
| Attribute       | Description                                |
+=================+============================================+
| ``channels``    | Channel count of the audio stream.         |
+-----------------+--------------------------------------------+
| ``sample_rate`` | Sample rate of the audio stream (Hz).      |
+-----------------+--------------------------------------------+
| ``bit_depth``   | Bit depth of the audio stream.             |
+-----------------+--------------------------------------------+
| ``ref``         | Reference timestamp (milliseconds UTC).    |
+-----------------+--------------------------------------------+
| ``size``        | Size of audio stream in bytes.             |
+-----------------+--------------------------------------------+

Dependencies
------------
- ``NONE``

Output
------

+------------+--------------------------------------------+
| Type       | Description                                |
+============+============================================+
| ``int``    | Value of the requested metadata attribute. |
+------------+--------------------------------------------+

Example
-------

.. code-block:: none

   INFO 'chopin' 'channels'
