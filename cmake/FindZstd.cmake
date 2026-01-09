# --- Find include directory ---
find_path(ZSTD_INCLUDE_DIR
        NAMES zstd.h
        PATH_SUFFIXES
        ""
        HINTS
        /opt/local/include
)

# --- Find the library ---
find_library(ZSTD_LIBRARY
        NAMES zstd
        HINTS
        /opt/local/lib
)

# --- Set variables for convenience ---
set(ZSTD_LIBRARIES ${ZSTD_LIBRARY})
set(ZSTD_INCLUDE_DIRS ${ZSTD_INCLUDE_DIR})

# --- Show messages ---
message(STATUS "Zstandard include: ${ZSTD_INCLUDE_DIR}")
message(STATUS "Zstandard library: ${ZSTD_LIBRARY}")
