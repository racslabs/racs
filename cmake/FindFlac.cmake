find_path (FLAC_INCLUDE_DIR stream_decoder.h stream_encoder.h
        PATH_SUFFIXES
        FLAC
        HINTS /opt/local/include
)

# Look for the library
find_library (FLAC_LIBRARY
        NAMES flac
        HINTS
        /opt/local/lib
)

set (FLAC_LIBRARIES ${FLAC_LIBRARY})
set (FLAC_INCLUDE_DIRS ${FLAC_INCLUDE_DIR})

message(STATUS "libflac: ${FLAC_LIBRARIES}")