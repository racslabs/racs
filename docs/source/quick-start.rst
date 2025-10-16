Quick Start
===========

The simplest way to run RACS is using ``docker``.

If you do not have docker on your machine, `Install Docker <https://docs.docker.com/get-started/get-docker/>`_ before continuing.

.. code-block:: bash

    docker build --tag racslabs/racs .

.. code-block:: bash

    docker run --network=host -p 6381:6381 racslabs/racs

You can now connect to the server on port 6381.

Other Build Options
-------------------

- :ref:`Build From Source <BUILD_FROM_SOURCE>`
