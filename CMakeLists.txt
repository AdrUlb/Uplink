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
		Source/Tosser/LList.hpp
		Source/ColourOption.hpp
		Source/OptionChange.hpp
		Source/Options.hpp
		Source/Options.cpp
		Source/Redshirt/Redshirt.hpp
		Source/Redshirt/Redshirt.cpp
		Source/Uplink.cpp
		Source/Uplink.hpp
		Source/App.cpp
		Source/PhoneDialler.hpp
		Source/MainMenu.hpp
		Source/Network.hpp)
add_executable(Uplink ${SOURCES})
target_compile_definitions(Uplink PRIVATE -DUPLINK_VERSION="${PROJECT_VERSION}")
