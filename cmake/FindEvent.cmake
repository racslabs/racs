# --- Find include directory ---
find_path(EVENT_INCLUDE_DIR
        NAMES event.h
        PATH_SUFFIXES
        event2
        ""          # Ubuntu puts headers directly in /usr/include
        HINTS
        /opt/local/include
)

# --- Find the library ---
find_library(EVENT_LIBRARY
        NAMES
        event
        HINTS
        /opt/local/lib
)

set(EVENT_LIBRARIES ${EVENT_LIBRARY})
set(EVENT_INCLUDE_DIRS ${EVENT_INCLUDE_DIR})

message(STATUS "libevent include: ${EVENT_INCLUDE_DIR}")
message(STATUS "libevent library: ${EVENT_LIBRARY}")
