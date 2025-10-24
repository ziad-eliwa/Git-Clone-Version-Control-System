project(google_tests)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(test
  test_vector.cpp
  test_hashmap.cpp
)
target_link_libraries(test PRIVATE DCVS_lib GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(test)
