add_library(Bungle STATIC Source/Bungle.cpp)
target_include_directories(Bungle PUBLIC Include)
target_link_libraries(Bungle PRIVATE crypto)