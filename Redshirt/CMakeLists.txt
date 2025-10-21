add_library(Redshirt STATIC Source/Redshirt.cpp)
target_include_directories(Redshirt PUBLIC Include)
target_link_libraries(Redshirt PRIVATE crypto PUBLIC Bungle)