Replication
===========

Current Limitations
-------------------

RACS currently supports basic master-slave replication, but with a few limitations.
Automatic failover is not implemented, backpressure is not handled, and slave nodes are not restricted to read-only mode. These limitations will be addressed in future releases.

Configuration
-------------

To enable replication, uncomment the relevant section in ``conf.yaml`` and specify the list of slave nodes with their host addresses and ports.

.. code-block:: yaml

    slaves:
      - host: 127.0.0.1
        port: 6382
