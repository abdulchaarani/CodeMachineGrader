cmake_minimum_required(VERSION 3.20)
project(CodeMachineGrader LANGUAGES CXX)

# Require C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Executable
add_executable(code_machine_grader main.cpp)

# Force console app (important for MinGW)
set_target_properties(code_machine_grader PROPERTIES WIN32_EXECUTABLE OFF)

# Compiler warnings for GCC/Clang
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(code_machine_grader PRIVATE -Wall -Wextra -Wpedantic -municode)
endif()
