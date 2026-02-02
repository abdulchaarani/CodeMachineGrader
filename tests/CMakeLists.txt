find_package(GTest QUIET)

if (NOT GTest_FOUND)
    message(STATUS "GTest not found. Fetching via FetchContent...")

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    # On Windows: prevent overriding compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
endif()

add_executable(run_tests
    test_code.cpp
)

target_link_libraries(run_tests
    GTest::gtest_main
    GTest::gtest
)

target_include_directories(run_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(run_tests PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
    )
endif()

include(GoogleTest)
gtest_discover_tests(run_tests)