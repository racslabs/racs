find_path (YAML_INCLUDE_DIR cyaml.h
        PATH_SUFFIXES
        /yaml
        HINTS /opt/local/include
)

# Look for the library
find_library (YAML_LIBRARY
        NAMES cyaml
        HINTS
        /opt/local/lib
)

set (YAML_LIBRARIES ${YAML_LIBRARY})
set (YAML_INCLUDE_DIRS ${YAML_INCLUDE_DIR})

message(STATUS "libYAML: ${YAML_LIBRARIES}")