cmake_minimum_required(VERSION 3.29)
project(Palette)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Palette
        include/Shell.h
        src/main/Shell.cpp
        include/Shell.h
        include/FileHandler.h
        )

target_compile_definitions(Palette PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

add_subdirectory(google_tests)
