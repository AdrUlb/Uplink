cmake_minimum_required(VERSION 4.1.2)

set(CMAKE_CXX_STANDARD 23)

string(APPEND CMAKE_C_FLAGS "-Wall -Wextra -pedantic")

project(Uplink VERSION 1.55 LANGUAGES C CXX)

add_subdirectory(Tosser)
add_subdirectory(Uplink)
