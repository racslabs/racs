
find_path(FLAC_INCLUDE_DIR
        NAMES FLAC/stream_decoder.h
)

find_library(FLAC_LIBRARY
        NAMES FLAC
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(FLAC
        REQUIRED_VARS
        FLAC_LIBRARY
        FLAC_INCLUDE_DIR
)

set(FLAC_LIBRARIES ${FLAC_LIBRARY})
set(FLAC_INCLUDE_DIRS ${FLAC_INCLUDE_DIR})

mark_as_advanced(
        FLAC_INCLUDE_DIR
        FLAC_LIBRARY
)