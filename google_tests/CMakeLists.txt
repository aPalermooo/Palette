cmake_minimum_required(VERSION 3.10)
project(Google_tests)

# Add Google Test library
add_subdirectory(lib)

# Create File Handler Tests Executable and link it with Google Test
add_executable(FHandlerTest unit_tests/FHandlerTest.cpp)

# Include your project headers
target_include_directories(FHandlerTest PRIVATE
        ${PROJECT_SOURCE_DIR}/../include
)

target_compile_definitions(FHandlerTest PRIVATE
        PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
)

# Link GoogleTest
target_link_libraries(
        FHandlerTest
        gtest
        gtest_main
)