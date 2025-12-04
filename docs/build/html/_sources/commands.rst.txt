Commands
========

Command Syntax
--------------

The command syntax follows this structure:

.. code-block:: none

   COMMAND <arg1> <arg2> ... <argN>

The command name is case-insensitive and arguments are separated by whitespace.

``string`` types must be enclosed in single or double quotes.

Pipe Operator ``|>``
--------------------

The pipe operator ``|>`` chains multiple commands together, passing the output of one command as input to the next.

.. code-block:: none

    COMMAND1 <arg1> <arg2> ... <argN> |> COMMAND2 <arg1> <arg2> ... <argN> |> ...

Certain commands, like ``FORMAT``, rely on input from a previous command and cannot run independently.

Commands
--------

:ref:`CLOSE`

:ref:`CREATE`

:ref:`EVAL`

:ref:`EXTRACT`

:ref:`FORMAT`

:ref:`INFO`

:ref:`SEARCH`

:ref:`OPEN`

:ref:`PING`

:ref:`SHUTDOWN`
