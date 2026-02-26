file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS Public/*.hpp)
add_library(Tosser INTERFACE ${SOURCES})
target_include_directories(Tosser INTERFACE Public)
