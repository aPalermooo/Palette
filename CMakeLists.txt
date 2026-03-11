cmake_minimum_required(VERSION 3.29)
project(Palette)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add libraries
add_subdirectory(lib/gTest)
add_subdirectory(lib/nlohmann_json)
add_subdirectory(google_tests)

# CLI BUILD
add_executable(Palette-CLI
        src/main/Shell.cpp
        src/components/explorer/FileHandler.cpp
        src/components/FileTagger.cpp
        src/components/explorer/DirectoryHandler.cpp
        src/components/explorer/Handler.cpp
        src/components/FileMover.cpp
        lib/SQLite/sqlite3.c
)

target_include_directories(Palette-CLI PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/SQLite
)

target_link_libraries(Palette-CLI PRIVATE
        nlohmann_json::nlohmann_json
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Palette-CLI PRIVATE
            PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
            FILE_PATH="${CMAKE_SOURCE_DIR}/env/Palette" # Override File System Location
            DB_PATH="${CMAKE_SOURCE_DIR}/env/palette.db" # Override Database Location
    )
else ()
    target_compile_definitions(Palette-CLI PRIVATE
            PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
            FILE_PATH=""
            DB_PATH=""
    )
endif ()

# GUI BUILD

add_executable(Palette
        src/main/Daemon.cpp
        src/components/explorer/FileHandler.cpp
        src/components/FileTagger.cpp
        src/components/explorer/DirectoryHandler.cpp
        src/components/explorer/Handler.cpp
        src/components/FileMover.cpp
        lib/SQLite/sqlite3.c
)

target_include_directories(Palette PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/SQLite
)

target_link_libraries(Palette PRIVATE
        nlohmann_json::nlohmann_json
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Palette PRIVATE
            PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
            FILE_PATH="${CMAKE_SOURCE_DIR}/env/Palette" # Override File System Location
            DB_PATH="${CMAKE_SOURCE_DIR}/env/palette.db" # Override Database Location
    )
else ()
    target_compile_definitions(Palette PRIVATE
            PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
            FILE_PATH=""
            DB_PATH=""
    )
endif ()

include(GoogleTest)
