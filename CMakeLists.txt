cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 23)

project(Uplink VERSION 1.55 LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(Tosser)
add_subdirectory(Redshirt)
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
		Source/Util.hpp)
add_executable(Uplink ${SOURCES})
target_compile_definitions(Uplink PRIVATE -DUPLINK_VERSION="${PROJECT_VERSION}")
target_link_libraries(Uplink PRIVATE Tosser Redshirt Gucci)
