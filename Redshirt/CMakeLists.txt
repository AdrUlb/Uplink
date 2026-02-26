file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS Source/*.cpp Source/*.hpp Public/*.hpp)
add_library(Redshirt STATIC ${SOURCES})
target_include_directories(Redshirt PUBLIC Public)
target_link_libraries(Redshirt PUBLIC Tosser Bungle)
