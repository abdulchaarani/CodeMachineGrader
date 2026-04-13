find_package(GTest QUIET)

if (NOT GTest_FOUND)
    message(STATUS "GTest not found. Fetching via FetchContent...")

    include(FetchContent)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_MakeAvailable(googletest)
endif()

###############
# Test ACC
###############
add_executable(test_ACC
    test_ACC.cpp
)

target_link_libraries(test_ACC
    GTest::gtest_main
    GTest::gtest
)

target_include_directories(test_ACC PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_definitions(test_ACC PRIVATE
    CODE_PATH_DEFAULT="${CMAKE_SOURCE_DIR}/code_examples/acc.s"
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(test_ACC PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
    )
endif()

include(GoogleTest)
gtest_discover_tests(test_ACC DISCOVERY_MODE PRE_TEST)

###############
# Test MA
###############
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

target_compile_definitions(test_MA PRIVATE
    CODE_PATH_DEFAULT="${CMAKE_SOURCE_DIR}/code_examples/ma.s"
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(test_MA PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
    )
endif()

include(GoogleTest)
gtest_discover_tests(test_MA DISCOVERY_MODE PRE_TEST)

###############
# Test PolyRisc
###############
add_executable(test_polyrisc
    test_polyrisc.cpp
)

target_link_libraries(test_polyrisc
    GTest::gtest_main
    GTest::gtest
)

target_include_directories(test_polyrisc PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_definitions(test_polyrisc PRIVATE
    CODE_PATH_DEFAULT="${CMAKE_SOURCE_DIR}/code_examples/polyrisc.s"
)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(test_polyrisc PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
    )
endif()

include(GoogleTest)
gtest_discover_tests(test_polyrisc DISCOVERY_MODE PRE_TEST)