cmake_minimum_required(VERSION 3.10)
project(Google_tests)

add_executable(Unit_Tests
        unit_tests/explorer/FileHandlerTest.cpp
        ${CMAKE_SOURCE_DIR}/src/components/explorer/Handler.cpp
        ${CMAKE_SOURCE_DIR}/src/components/explorer/FileHandler.cpp
        ${CMAKE_SOURCE_DIR}/src/components/explorer/DirectoryHandler.cpp
        unit_tests/explorer/DirectoryHandlerTest.cpp
        unit_tests/explorer/AbstractHandlerTest.cpp
)

target_include_directories(Unit_Tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/google_tests/unit_tests/explorer
)

target_compile_definitions(Unit_Tests PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(Unit_Tests PRIVATE
        GTest::gtest_main
        nlohmann_json::nlohmann_json
)

include(GoogleTest)
gtest_discover_tests(Unit_Tests)