set(SOURCES
		Source/Eclipse.cpp
		Include/Eclipse/Eclipse.hpp)
add_library(Eclipse STATIC ${SOURCES})
target_include_directories(Eclipse PUBLIC Include)
