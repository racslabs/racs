.. _SCM_INFO:

INFO
====

**Description:**
Retrieves metadata attributes of an audio stream.

Arguments
---------

+-------------+------------+---------------------------------------------------------------+
| Name        | Type       | Description                                                   |
+=============+============+===============================================================+
| stream_id   | ``String`` | ASCII ID of the audio stream.                                 |
+-------------+------------+---------------------------------------------------------------+
| attribute   | ``String`` | Name of the metadata attribute to retrieve.                   |
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

Output
------

+-------------+--------------------------------------------+
| Type        | Description                                |
+=============+============================================+
| ``Integer`` | Value of the requested metadata attribute. |
+-------------+--------------------------------------------+

Example
-------

.. code-block:: scheme

    ;; SCM expression
    (info "chopin" "channels")

Using ``EVAL``:

.. code-block:: none

    EVAL '(info \"chopin\" \"channels\")'
