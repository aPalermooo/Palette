cmake_minimum_required(VERSION 3.29)
project(Palette)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add libraries
add_subdirectory(lib/google_test)
add_subdirectory(lib/json)
add_subdirectory(google_tests)

include_directories(include .)

add_executable(Palette
        src/main/Shell.cpp
        src/components/FileHandler.cpp
        src/FileTagger.cpp
)

target_link_libraries(Palette PRIVATE
        nlohmann_json::nlohmann_json)


target_compile_definitions(Palette PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

include(GoogleTest)
