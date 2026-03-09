cmake_minimum_required(VERSION 3.29)
project(Palette)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(nlohmann_json CONFIG REQUIRED)

include_directories(.)

add_executable(Palette
        src/main/Shell.cpp
        src/FileTagger.cpp
        include/FileHandler.h
        include/FileTagger.h
)

target_link_libraries(Palette PRIVATE nlohmann_json::nlohmann_json)

target_compile_definitions(Palette PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

add_subdirectory(google_tests)
