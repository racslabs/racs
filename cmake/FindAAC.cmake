find_path (AAC_INCLUDE_DIR aacdecoder_lib.h aacenc_lib.h
        PATH_SUFFIXES
        fdk-aac
        HINTS /opt/local/include
)

# Look for the library
find_library (AAC_LIBRARY
        NAMES fdk-aac
        HINTS
        /opt/local/lib
)

set (AAC_LIBRARIES ${AAC_LIBRARY})
set (AAC_INCLUDE_DIRS ${AAC_INCLUDE_DIR})

message(STATUS "libfdk-aac: ${AAC_LIBRARIES}")