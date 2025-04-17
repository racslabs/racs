find_path (OPUSENC_INCLUDE_DIR opusenc.h
        PATH_SUFFIXES
        opus
        HINTS /opt/local/include
)

# Look for the library
find_library (OPUSENC_LIBRARY
        NAMES opusenc
        HINTS
        /opt/local/lib
)

set (OPUSENC_LIBRARIES ${OPUSENC_LIBRARY})
set (OPUSENC_INCLUDE_DIRS ${OPUSENC_INCLUDE_DIR})

message(STATUS "libopusenc: ${OPUSENC_LIBRARIES}")
