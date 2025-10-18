cmake_minimum_required(VERSION 4.0)

project(Uplink VERSION 1.55 DESCRIPTION "Uplink decompilation" LANGUAGES CXX)

add_executable(Uplink Source/Main.cpp Source/UplinkObject.cpp)
target_compile_options(Uplink PRIVATE -O2 -g)
install(TARGETS Uplink)