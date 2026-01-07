Getting Started
===============

Use one of the following options to install and run RACS:

.. toctree::
   :maxdepth: 1

   install-with-docker
   build-from-source

Hardware Compatability
----------------------

RACS supports both x86_64 and arm64 architectures.

For x86_64, Sandy Bridge is the minimum CPU required to run RACS. For Intel, Haswell is recommended, and for AMD, Excavator or newer is advised, as SIMD-optimized DSP commands will be added in the near future.

For arm64, the host byte order must be little-endian.

OS Compatability
----------------

RACS is compatible with Linux Kernel 4.19+ and macOS 10.15+.