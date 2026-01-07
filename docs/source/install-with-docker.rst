.. _INSTALL_WITH_DOCKER

Install with Docker
===================

If you do not have Docker on your machine, `install Docker <https://docs.docker.com/get-started/get-docker/>`_ before continuing.

Prerequisites
-------------

- Minimum 4 GB of RAM
- Minimum 1 CPU core
- Linux Kernel 4.19+ or macOS 10.15+

Running RACS
------------

To start the RACS server with Docker, run the command below:

.. code-block:: shell

    docker run -p 6381:6381 racslabs/racs

Once started, the server listens on ``localhost:6381`` by default.