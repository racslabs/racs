# --- Find libbsd include dir ---
find_path(BSD_INCLUDE_DIR
        NAMES bsd/string.h
        PATH_SUFFIXES
        bsd
        HINTS
        /usr/include
        /opt/local/include
)

# --- Find libbsd library ---
find_library(BSD_LIBRARY
        NAMES bsd
        HINTS
        /usr/lib
        /usr/lib/x86_64-linux-gnu
        /opt/local/lib
)

# --- Expose variables ---
set(BSD_LIBRARIES ${BSD_LIBRARY})
set(BSD_INCLUDE_DIRS ${BSD_INCLUDE_DIR})

# --- Print results ---
message(STATUS "libbsd include: ${BSD_INCLUDE_DIR}")
message(STATUS "libbsd library: ${BSD_LIBRARY}")
