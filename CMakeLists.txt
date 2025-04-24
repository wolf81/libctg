cmake_minimum_required(VERSION 3.10)
project(ctg C)

set(CMAKE_C_STANDARD 99)

# Silence non-secure sscanf warning when compiling on Windows
if (MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

# Path to LuaJIT source (including headers)
set(LUAJIT_SRC_DIR "${CMAKE_SOURCE_DIR}/deps/luajit/src")

# Path to LuaJIT built products
set(LUAJIT_BUILD_DIR "${CMAKE_SOURCE_DIR}/deps/luajit/build/src/Debug")

if (WIN32)
    if (NOT EXISTS "${LUAJIT_SRC_DIR}/lua51.lib")
        message(FATAL_ERROR "Missing library ${LUAJIT_SRC_DIR}/lua51.lib - execute build_luajit.bat")
    else()
        set(LUAJIT_LIB "${LUAJIT_SRC_DIR}/lua51.lib")
    endif()
endif()

if(APPLE)
    if (NOT EXISTS "${LUAJIT_BUILD_DIR}/libluajit.a")
        message(FATAL_ERROR "Missing library ${LUAJIT_BUILD_DIR}/libluajit.a - execute build_luajit.sh")
    else()
        set(LUAJIT_LIB "${LUAJIT_BUILD_DIR}/libluajit.a")
    endif()
endif()

# Add source directories
include_directories(src ${LUAJIT_SRC_DIR})

# Create a shared library
add_library(libctg SHARED
    src/libctg.c
    src/libctg_lua.c
)

# Always produce libctg.dll / libctg.so / libctg.dylib
set_target_properties(libctg PROPERTIES OUTPUT_NAME "ctg" PREFIX "lib")

if (APPLE)
	# Ensure that Lua symbols are found dynamically
	set_target_properties(libctg PROPERTIES
        LINK_FLAGS "-undefined dynamic_lookup"
	)
endif()

if (WIN32)
	# Link LuaJIT to libctg - dynamic lookup is not supported I think ...
	target_link_libraries(libctg PRIVATE ${LUAJIT_LIB})
endif()

# Create a main test binary
add_executable(test src/main.c)

# Link test executable with library
target_link_libraries(test libctg)

# Ensure test sees library header files
target_include_directories(test PRIVATE src)
