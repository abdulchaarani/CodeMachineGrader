cmake_minimum_required(VERSION 3.20)
project(CodeMachineGrader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable compile commands for IntelliSense
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Main executable
add_executable(code_machine_grader 
    src/main.cpp
)

set_target_properties(code_machine_grader PROPERTIES WIN32_EXECUTABLE OFF)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(code_machine_grader PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic 
        -finput-charset=UTF-8
        $<$<CONFIG:Debug>:-g3>
        $<$<CONFIG:Debug>:-O0>
    )
endif()

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()