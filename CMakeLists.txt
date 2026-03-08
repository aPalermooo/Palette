cmake_minimum_required(VERSION 3.29)
project(Palette)

set(CMAKE_CXX_STANDARD 20)


include_directories(.)

add_executable(Palette
        src/main/Shell.cpp
        include/FileHandler.h
)

target_compile_definitions(Palette PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

add_subdirectory(google_tests)
