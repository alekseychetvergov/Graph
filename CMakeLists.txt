cmake_minimum_required(VERSION 3.14)
project(Graph VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(Graph main.cpp)

include(CTest)

if(BUILD_TESTING)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
        GIT_SHALLOW TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(GraphTests tests/test_graph.cpp)
    target_include_directories(GraphTests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
    target_link_libraries(GraphTests PRIVATE GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(GraphTests)
endif()
