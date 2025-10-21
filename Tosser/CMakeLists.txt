set(SOURCES
		Source/Tosser.cpp
		Include/Tosser/BTree.hpp
		Include/Tosser/DArray.hpp
		Include/Tosser/LList.hpp)
add_library(Tosser STATIC ${SOURCES})
target_include_directories(Tosser PUBLIC Include)
