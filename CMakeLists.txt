cmake_minimum_required(VERSION 4.0)

set(CMAKE_CXX_STANDARD 23)

project(Uplink VERSION 1.55 LANGUAGES CXX)

include_directories(${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(Tosser)
add_subdirectory(Redshirt)
add_subdirectory(Gucci)

set(SOURCES
		Source/Main.cpp
		Source/UplinkObject.cpp
		Source/Option.cpp
		Source/Util.cpp
		Source/ColourOption.hpp
		Source/OptionChange.hpp
		Source/Options.hpp
		Source/Options.cpp
		Source/Uplink.cpp
		Source/Uplink.hpp
		Source/App.cpp
		Source/PhoneDialler.hpp
		Source/MainMenu.hpp
		Source/Network.hpp)
add_executable(Uplink ${SOURCES})
target_compile_definitions(Uplink PRIVATE -DUPLINK_VERSION="${PROJECT_VERSION}")
target_link_libraries(Uplink PRIVATE Tosser Redshirt Gucci crypto)
