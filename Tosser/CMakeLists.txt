add_library(Tosser STATIC Source/Tosser.cpp Include/Tosser/BTree.hpp Include/Tosser/DArray.hpp Include/Tosser/LList.hpp)
target_include_directories(Tosser PUBLIC Include)
