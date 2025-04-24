cmake_minimum_required(VERSION 3.10)
project(ClearTheGrid C)

set(CMAKE_C_STANDARD 99)

# Silence non-secure sscanf warning when compiling on Windows
if (MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# Add source directory
include_directories(src)

# Create a shared library
add_library(libctg SHARED
    src/libctg.c
)

# Create a main test binary
add_executable(test src/main.c)

# Link test executable with library
target_link_libraries(test libctg)

# Ensure test sees library header files
target_include_directories(test PRIVATE src)
