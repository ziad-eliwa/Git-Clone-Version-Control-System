cmake_minimum_required(VERSION 3.24)
project(distributed_version_control_system)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_library(DCVS_lib 
  src/argparser.cpp
  src/argparser.h
  src/deque.h
  src/diff.cpp
  src/diff.h
  src/hashmap.cpp
  src/hashmap.h
  src/helpers.cpp
  src/helpers.h
  src/index.h
  src/objectstore.cpp
  src/objectstore.h
  src/refs.cpp
  src/refs.h
  src/vector.h
)
target_include_directories(DCVS_lib PUBLIC src) 


add_executable(jit
  main.cpp
)
target_link_libraries(jit PRIVATE DCVS_lib)

add_subdirectory(tests)



