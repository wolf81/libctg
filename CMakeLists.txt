cmake_minimum_required(VERSION 3.10)
project(ClearTheGrid C)

set(CMAKE_C_STANDARD 99)

# Silence non-secure sscanf warning when compiling on Windows
if (MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

if (WIN32 AND MSVC AND NOT EXISTS "${LUAJIT_SRC_DIR}/lua51.lib")
    message(STATUS "Running msvcbuild.bat for LuaJIT")

    execute_process(
        COMMAND cmd /c "cd /d ${CMAKE_SOURCE_DIR}/deps/luajit/src && msvcbuild.bat"
        RESULT_VARIABLE result
    )

    if (NOT result EQUAL 0)
        message(FATAL_ERROR "LuaJIT msvcbuild.bat failed with code ${result}")
    endif()
endif()

# Path to Lua headers
set(LUAJIT_SRC_DIR "${CMAKE_SOURCE_DIR}/deps/luajit/src")

# Add source directory
include_directories(src ${LUAJIT_SRC_DIR})

# Link against the import library (not the DLL)
set(LUAJIT_LIB "${LUAJIT_SRC_DIR}/lua51.lib")

# Create a shared library
add_library(libctg SHARED
    src/libctg.c
    src/libctg_lua.c
)

# Link LuaJIT to libctg
target_link_libraries(libctg PRIVATE ${LUAJIT_LIB})

# Create a main test binary
add_executable(test src/main.c)

# Link test executable with library
target_link_libraries(test libctg)

# Ensure test sees library header files
target_include_directories(test PRIVATE src)
