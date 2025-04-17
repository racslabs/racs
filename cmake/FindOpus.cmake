find_path (OPUS_INCLUDE_DIR opus.h
        PATH_SUFFIXES
        opus
        HINTS /opt/local/include
)

# Look for the library
find_library (OPUS_LIBRARY
        NAMES opus
        HINTS
        /opt/local/lib
)

set (OPUS_LIBRARIES ${OPUS_LIBRARY})
set (OPUS_INCLUDE_DIRS ${OPUS_INCLUDE_DIR})

message(STATUS "libopus: ${OPUS_LIBRARIES}")
