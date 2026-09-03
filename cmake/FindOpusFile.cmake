find_path (OPUSFILE_INCLUDE_DIR opusfile.h
        PATH_SUFFIXES
        opus
        HINTS /opt/local/include
)

# Look for the library
find_library (OPUSFILE_LIBRARY
        NAMES opusfile
        HINTS
        /opt/local/lib
)

set (OPUSFILE_LIBRARIES ${OPUSFILE_LIBRARY})
set (OPUSFILE_INCLUDE_DIRS ${OPUSFILE_INCLUDE_DIR})

message(STATUS "opusfile: ${OPUSFILE_LIBRARIES}")