Getting Started
===============

The quickest way to run RACS is using docker.

If you do not have docker on your machine, `Install Docker <https://docs.docker.com/get-started/get-docker/>`_ before continuing.

.. code-block:: shell

    docker build --tag docker.racslabs.com/racs .

.. code-block:: shell

    docker run --network=host -p 6381:6381 docker.racslabs.com/racs

You can now connect to the server on port 6381.

Other Build Options
-------------------

- :ref:`Build From Source <BUILD_FROM_SOURCE>`
