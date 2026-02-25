file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS Source/*.cpp Source/*.hpp)
add_executable(Uplink ${SOURCES})
target_compile_definitions(Uplink PRIVATE -DUPLINK_VERSION="${PROJECT_VERSION}")
