find_package(GTest REQUIRED)

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