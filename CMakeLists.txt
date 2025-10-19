cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 23)

project(Uplink VERSION 1.55 DESCRIPTION "Uplink decompilation" LANGUAGES CXX)

set(SOURCES
		Source/Main.cpp
		Source/UplinkObject.cpp
		Source/Option.cpp
		Source/Util.cpp
		Source/Tosser/DArray.hpp
		Source/Tosser/BTree.hpp
		Source/Tosser/LList.hpp)
add_executable(Uplink ${SOURCES})
