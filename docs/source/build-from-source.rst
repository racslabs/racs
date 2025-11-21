.. _BUILD_FROM_SOURCE:

Build from Source
=================

RACS can be built on ``Linux`` and ``macOS``.
The recommended compiler is ``GCC 7+``.
Supported CPU Architectures: ``x86/x86_64``, ``arm64`` (Little endian only)

Install Dependencies
--------------------

**Debian/Ubuntu:**

.. code-block:: bash

    sudo apt install ninja-build cmake g++ libmp3lame-dev libopus-dev libopusenc-dev \
        guile-3.0-dev libmsgpack-dev libcyaml-dev

``libopusenc-dev`` is not available on Ubuntu. To install it from source, run the following:

.. code-block:: bash

    git clone https://github.com/xiph/libopusenc.git && cd libopusenc
    ./autogen.sh
    ./configure
    make -j$(nproc)
    sudo make install

**macOS:**

.. code-block:: bash

    brew install cmake ninja pkg-config gcc@13 guile lame msgpack opus \
        libopusenc libcyaml


Clone the Repository
--------------------

.. code-block:: bash

    git clone --branch main https://github.com/racslabs/racs.git && cd racs


Build the Project
-----------------

.. code-block:: bash

    cmake -B build -G Ninja && cmake --build build --target racs


Run RACS
--------

Run the RACS executable from the build directory, specifying the path to the ``conf.yaml`` file located in the project root:

.. code-block:: bash

    build/racs --config conf.yaml

Once started, the server listens on ``localhost:6381`` by default.
