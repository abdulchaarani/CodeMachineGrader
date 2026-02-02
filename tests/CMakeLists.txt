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

add_executable(test_MA
    test_MA.cpp
)

target_link_libraries(test_MA
    GTest::gtest_main
    GTest::gtest
)

target_include_directories(test_MA PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(test_MA PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
    )
endif()

include(GoogleTest)
gtest_discover_tests(test_MA)