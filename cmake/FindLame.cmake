find_path (LAME_INCLUDE_DIR lame.h
        PATH_SUFFIXES
        lame
        HINTS /opt/local/include
)

# Look for the library
find_library (LAME_LIBRARY
        NAMES lame mp3lame
        HINTS
        /opt/local/lib
)

set (LAME_LIBRARIES ${LAME_LIBRARY})
set (LAME_INCLUDE_DIRS ${LAME_INCLUDE_DIR})

message(STATUS "liblame: ${LAME_LIBRARIES}")
