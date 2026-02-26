file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS Source/*.cpp Public/*.hpp)
add_library(Tosser STATIC ${SOURCES})
target_include_directories(Tosser PUBLIC Public)
