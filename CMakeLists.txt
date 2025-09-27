# https://google.github.io/googletest/quickstart-cmake.html
# https://code.visualstudio.com/docs/cpp/cmake-quickstart

cmake_minimum_required(VERSION 3.14)
project(brainfuck-turing VERSION 0.1.0 LANGUAGES C CXX)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip # 30.04.2025r.
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB SRC_FILES "src/*.cpp")
file(GLOB TESTS "tests/*.cpp")

add_executable(
  main
  main.cpp
  ${SRC_FILES}
)

add_subdirectory(tests)