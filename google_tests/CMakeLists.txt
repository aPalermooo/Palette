cmake_minimum_required(VERSION 3.10)
project(Google_tests)

# Add Google Test library
add_subdirectory(lib)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
