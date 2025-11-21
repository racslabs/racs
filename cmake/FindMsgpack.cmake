# --- Find include directory ---
find_path(MSGPACK_INCLUDE_DIR
        NAMES msgpack.h
        PATH_SUFFIXES
        msgpack-c
        msgpack
        ""          # Ubuntu puts headers directly in /usr/include
        HINTS
        /opt/local/include
)

# --- Find the library ---
find_library(MSGPACK_LIBRARY
        NAMES
        msgpackc          # Ubuntu
        msgpack-c         # Debian
        msgpack           # fallback / older distros
        HINTS
        /opt/local/lib
)

set(MSGPACK_LIBRARIES ${MSGPACK_LIBRARY})
set(MSGPACK_INCLUDE_DIRS ${MSGPACK_INCLUDE_DIR})

message(STATUS "msgpack include: ${MSGPACK_INCLUDE_DIR}")
message(STATUS "msgpack library: ${MSGPACK_LIBRARY}")
