Syntax
======

Command syntax
--------------

The command syntax follows this structure:

``COMMAND <arg1> <arg2> ... <argN>``

The command name are case-insensitive and arguments are separated by whitespace.

``string`` types must be enclosed in single or double quotes.

Pipe Operator ``|>``
--------------------

The pipe operator ``|>`` chains multiple commands together, passing the output of one command as input to the next.

``COMMAND1 <arg1> <arg2> ... <argN> |> ... |> COMMANDN <arg1> <arg2> ... <argN>``

Certain commands, like ``FORMAT``, rely on input from a previous command and cannot run independently, while others must execute beforehand.
