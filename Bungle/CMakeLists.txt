file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS Source/*.cpp Source/*.hpp Public/*.hpp)
add_library(Bungle STATIC ${SOURCES})
target_include_directories(Bungle PUBLIC Public)
target_link_libraries(Bungle PUBLIC Tosser)
