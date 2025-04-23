cmake_minimum_required(VERSION 3.10)
project(ClearTheGrid C)

set(CMAKE_C_STANDARD 99)

# Add source directory
include_directories(src)

# Create the library
add_library(ctg STATIC src/libctg.c)

# Create a main test binary
add_executable(ctg_main src/main.c)
target_link_libraries(ctg_main ctg)
