find_path (MPG123_INCLUDE_DIR mpg123.h
        PATH_SUFFIXES
        mpg123
        HINTS /opt/local/include
)

# Look for the library
find_library (MPG123_LIBRARY
        NAMES mpg123
        HINTS
        /opt/local/lib
)

set (MPG123_LIBRARIES ${MPG123_LIBRARY})
set (MPG123_INCLUDE_DIRS ${MPG123_INCLUDE_DIR})

message(STATUS "libmpg123: ${MPG123_LIBRARIES}")
