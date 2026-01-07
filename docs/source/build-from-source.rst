.. _BUILD_FROM_SOURCE:

Build from source
=================

Prerequisites
-------------

- Minimum 4GB of RAM
- Minimum 1 CPU Core
- Linux Kernel 4.19+ or macOS 10.15+

Install dependencies
--------------------

On Debian/Ubuntu
^^^^^^^^^^^^^^^^

.. code-block:: bash

    sudo apt install ninja-build cmake g++ libmp3lame-dev libopus-dev libopusenc-dev \
        guile-3.0-dev libmsgpack-dev libcyaml-dev libevent-dev libzstd-dev

``libopusenc-dev`` is not available on Ubuntu. To install it from source, run the following:

.. code-block:: bash

    git clone https://github.com/xiph/libopusenc.git && cd libopusenc
    ./autogen.sh
    ./configure
    make -j$(nproc)
    sudo make install

On macOS
^^^^^

.. code-block:: bash

    brew install cmake ninja pkg-config gcc@13 guile lame msgpack opus \
        libopusenc libcyaml libevent


Clone the repository
--------------------

.. code-block:: bash

    git clone --branch main https://github.com/racslabs/racs.git && cd racs


Build the project
-----------------

.. code-block:: bash

    cmake -B build -G Ninja && cmake --build build --target racs


Run RACS
--------

Run the RACS executable from the build directory, specifying the path to the ``conf.yaml`` file located in the project root:

.. code-block:: bash

    build/racs --config conf.yaml

Once started, the server listens on ``localhost:6381`` by default.
