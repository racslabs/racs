find_path (MSGPACK_INCLUDE_DIR msgpack.h
        PATH_SUFFIXES
        msgpack-c
        HINTS /opt/local/include
)

# Look for the library
find_library (MSGPACK_LIBRARY
        NAMES msgpack-c
        HINTS
        /opt/local/lib
)

set (MSGPACK_LIBRARIES ${MSGPACK_LIBRARY})
set (MSGPACK_INCLUDE_DIRS ${MSGPACK_INCLUDE_DIR})

message(STATUS "msgpack: ${MSGPACK_LIBRARIES}")