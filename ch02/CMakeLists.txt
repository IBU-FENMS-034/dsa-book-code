# The smallest CMakeLists.txt that does something real. Chapter 2 reads this
# file line by line, so keep it minimal and keep the comments accurate.
cmake_minimum_required(VERSION 3.20)

project(Chapter02 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# One executable, built from two translation units.
add_executable(ch02_tests
        tests.cpp
        Student.cpp)

# Turn the warnings on. Everything in this book compiles clean with them.
target_compile_options(ch02_tests PRIVATE -Wall -Wextra)

enable_testing()
add_test(NAME ch02 COMMAND ch02_tests)
