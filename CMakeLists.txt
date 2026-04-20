cmake_minimum_required(VERSION 3.29)
project(Palette)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add libraries
add_subdirectory(backend/lib/gTest)
add_subdirectory(backend/lib/nlohmann_json)
add_subdirectory(backend/google_tests)

include_directories(${CMAKE_SOURCE_DIR}/backend/lib/crow)
include_directories(${CMAKE_SOURCE_DIR}/backend/lib/asio)





# CLI BUILD
add_executable(Palette-CLI
        backend/src/main/Shell.cpp
        backend/src/components/explorer/FileHandler.cpp
        backend/src/components/FileTagger.cpp
        backend/src/components/explorer/DirectoryHandler.cpp
        backend/src/components/explorer/Handler.cpp
        backend/src/components/FileMover.cpp
        backend/lib/SQLite/sqlite3.c
)

target_include_directories(Palette-CLI PRIVATE
        ${CMAKE_SOURCE_DIR}/backend/include
        ${CMAKE_SOURCE_DIR}/backend/lib/SQLite
)

target_link_libraries(Palette-CLI PRIVATE
        nlohmann_json::nlohmann_json
        ws2_32
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Palette-CLI PRIVATE
            PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
            FILE_PATH="${CMAKE_SOURCE_DIR}/env/Palette"
            DB_PATH="${CMAKE_SOURCE_DIR}/env/palette.db"
    )
else()
    target_compile_definitions(Palette-CLI PRIVATE
            _WIN32_WINNT=0x0601
            PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
            FILE_PATH=""
            DB_PATH=""
    )
endif()
##############################################



# GUI BUILD
add_executable(Palette
        backend/src/main/Daemon.cpp
        backend/src/components/explorer/FileHandler.cpp
        backend/src/components/FileTagger.cpp
        backend/src/components/explorer/DirectoryHandler.cpp
        backend/src/components/explorer/Handler.cpp
        backend/src/components/FileMover.cpp
        backend/lib/SQLite/sqlite3.c
        backend/src/components/trayIcon.cpp
)

target_include_directories(Palette PRIVATE
        ${CMAKE_SOURCE_DIR}/backend/include
        ${CMAKE_SOURCE_DIR}/backend/lib/SQLite
)

target_link_libraries(Palette PRIVATE
        nlohmann_json::nlohmann_json
        ws2_32
        mswsock
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(Palette PRIVATE
            PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
            FILE_PATH="${CMAKE_SOURCE_DIR}/env/Palette"
            DB_PATH="${CMAKE_SOURCE_DIR}/env/palette.db"
    )
else()
    target_compile_definitions(Palette PRIVATE
            PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
            FILE_PATH=""
            DB_PATH=""
    )
endif()
#######################################

include(GoogleTest)