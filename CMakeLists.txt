cmake_minimum_required(VERSION 3.24)
project(distributed_version_control_system)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(DCVS_lib 
  src/vector.h
  src/hashmap.cpp
  src/hashmap.h
  src/Trie.cpp
  src/Trie.h
)
target_include_directories(DCVS_lib PUBLIC src) 

add_executable(jit
  src/main.cpp
)

add_subdirectory(tests)



