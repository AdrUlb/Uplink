cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 23)

add_compile_options(-Wall -Wextra)
add_compile_options(-Wno-unused-variable -Wno-unused-parameter)

project(Uplink VERSION 1.55 LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(Tosser)
add_subdirectory(Bungle)
add_subdirectory(Redshirt)
add_subdirectory(Eclipse)
add_subdirectory(Gucci)

set(SOURCES
		Source/App.cpp
		Source/App.hpp
		Source/ColourOption.hpp
		Source/Main.cpp
		Source/MainMenu.hpp
		Source/Network.hpp
		Source/Option.cpp
		Source/Option.hpp
		Source/OptionChange.hpp
		Source/Options.cpp
		Source/Options.hpp
		Source/PhoneDialler.hpp
		Source/Uplink.cpp
		Source/Uplink.hpp
		Source/UplinkObject.cpp
		Source/UplinkObject.hpp
		Source/Util.cpp
		Source/Util.hpp
		Source/VLocation.hpp
		Source/VLocation.cpp
		Source/Date.hpp
		Source/Date.cpp)
add_executable(Uplink ${SOURCES})
target_compile_definitions(Uplink PRIVATE -DUPLINK_VERSION="${PROJECT_VERSION}")

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
	target_compile_definitions(Uplink PRIVATE -DUPLINK_TYPE_RELEASE -DUPLINK_TYPE="RELEASE")
else ()
	target_compile_definitions(Uplink PRIVATE -DUPLINK_TYPE_DEBUG -DUPLINK_TYPE="DEBUG" -DUPLINK_DEBUG)
endif ()

target_link_libraries(Uplink PRIVATE Tosser Redshirt Eclipse Gucci)
