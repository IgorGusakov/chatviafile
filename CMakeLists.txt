cmake_minimum_required(VERSION 3.13)
project(testfile)


set(CMAKE_CXX_STANDARD 17)

##CONAN
#include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
#conan_basic_setup()

find_package(Threads REQUIRED)
find_package(Poco REQUIRED Foundation)
add_executable(testfile main.cpp WorkWithFile.h WorkWithFile.cpp)
target_link_libraries(testfile PRIVATE Threads::Threads)
target_link_libraries(testfile PRIVATE Poco::Foundation)
#target_link_libraries(testfile ${CONAN_LIBS})
###########################################################################
# Unit Test
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
find_package(Poco REQUIRED Foundation)
enable_testing()
add_executable(unit_test
        unit-test/test_file.cpp WorkWithFile.cpp)

target_link_libraries(unit_test
        gtest_main
        gtest
        gmock
        Poco::Foundation
)

include(GoogleTest)

gtest_discover_tests(unit_test)