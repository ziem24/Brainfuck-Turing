file(GLOB TEST_FILES "*.cpp")
file(GLOB_RECURSE SRC_FILES "../src/*.cpp")

add_executable(
  gtest_dir
  ${TEST_FILES}
  ${SRC_FILES}
)

target_include_directories(gtest_dir PRIVATE ../src)

target_link_libraries(
  gtest_dir
  GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(gtest_dir)