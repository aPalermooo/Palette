cmake_minimum_required(VERSION 3.10)
project(Google_tests)

add_executable(FHandlerTest
        unit_tests/FHandlerTest.cpp
        ${CMAKE_SOURCE_DIR}/src/FileTagger.cpp
)

target_include_directories(FHandlerTest PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

target_compile_definitions(FHandlerTest PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

target_link_libraries(FHandlerTest PRIVATE
        GTest::gtest_main
        nlohmann_json::nlohmann_json
)

include(GoogleTest)
gtest_discover_tests(FHandlerTest)