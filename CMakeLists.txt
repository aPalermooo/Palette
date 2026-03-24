cmake_minimum_required(VERSION 3.29)
project(Palette)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(unofficial-sqlite3 CONFIG REQUIRED)

# Add libraries
add_subdirectory(lib/gTest)
add_subdirectory(google_tests)

add_executable(Palette
        src/main/Shell.cpp
        src/components/explorer/FileHandler.cpp
        src/components/FileTagger.cpp
        src/components/explorer/DirectoryHandler.cpp
        include/explorer/Handler.h
        src/components/explorer/Handler.cpp
        src/components/FileMover.cpp
        include/FileMover.h
)

target_include_directories(Palette PRIVATE
        ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(Palette PRIVATE
        unofficial::sqlite3::sqlite3)


target_compile_definitions(Palette PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

include(GoogleTest)
