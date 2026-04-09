cmake_minimum_required(VERSION 3.10)
project(Google_tests)

include(GoogleTest)

set(EXPLORER_SOURCES
        ${CMAKE_SOURCE_DIR}/src/components/explorer/Handler.cpp
        ${CMAKE_SOURCE_DIR}/src/components/explorer/FileHandler.cpp
        ${CMAKE_SOURCE_DIR}/src/components/explorer/DirectoryHandler.cpp
)

set(EXPLORER_INCLUDES
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/google_tests/unit_tests/explorer
)

add_executable(AbstractHandlerTest
        unit_tests/explorer/AbstractHandlerTest.cpp
        ${EXPLORER_SOURCES}
)

add_executable(FileHandlerTest
        unit_tests/explorer/FileHandlerTest.cpp
        ${EXPLORER_SOURCES}
)

add_executable(DirectoryHandlerTest
        unit_tests/explorer/DirectoryHandlerTest.cpp
        ${EXPLORER_SOURCES}
)

foreach(target AbstractHandlerTest FileHandlerTest DirectoryHandlerTest)
    target_include_directories(${target} PRIVATE ${EXPLORER_INCLUDES})
    target_compile_definitions(${target} PRIVATE PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
    target_link_libraries(${target} PRIVATE GTest::gtest_main nlohmann_json::nlohmann_json)
    gtest_discover_tests(${target})
endforeach()