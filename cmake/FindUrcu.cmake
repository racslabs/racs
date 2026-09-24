
find_path(URCU_INCLUDE_DIR urcu-qsbr.h
        PATH_SUFFIXES
        opus
        HINTS /opt/local/include
)

# Look for the library
find_library (URCU_LIBRARY
        NAMES urcu-qsbr
        HINTS
        /opt/local/lib
)

set (URCU_LIBRARIES ${URCU_LIBRARY})
set (URCU_INCLUDE_DIRS ${URCU_INCLUDE_DIR})

message(STATUS "urcu: ${URCU_LIBRARIES}")