cmake_minimum_required(VERSION 3.13)
project(testfile)

set(CMAKE_CXX_STANDARD 17)

add_executable(testfile main.cpp WorkWithFile.h WorkWithFile.cpp)


###########################################################################
# Unit Test
find_package(Threads REQUIRED)

include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.12.1
)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
    FetchContent_Populate(googletest)
    add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()

enable_testing()
add_executable(unit_test
        unit-test/test_file.cpp WorkWithFile.cpp)

target_link_libraries(unit_test
        gtest_main
        gtest
        gmock
)

include(GoogleTest)

gtest_discover_tests(unit_test)