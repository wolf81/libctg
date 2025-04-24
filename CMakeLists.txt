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

# macOS: build LuaJIT using make to produce libluajit.a
if(APPLE)
    set(LUAJIT_BUILD_DIR "${CMAKE_SOURCE_DIR}/deps/luajit")
    set(LUAJIT_OUTPUT_LIB "${LUAJIT_SRC_DIR}/libluajit.a")

    add_custom_command(
        OUTPUT ${LUAJIT_OUTPUT_LIB}
        COMMAND make MACOSX_DEPLOYMENT_TARGET=10.14
        WORKING_DIRECTORY ${LUAJIT_BUILD_DIR}
        COMMENT "Building LuaJIT on macOS"
    )

    add_custom_target(luajit_build DEPENDS ${LUAJIT_OUTPUT_LIB})
endif()

# Path to Lua headers
set(LUAJIT_SRC_DIR "${CMAKE_SOURCE_DIR}/deps/luajit/src")

# Add source directory
include_directories(src ${LUAJIT_SRC_DIR})

# Select the correct LuaJIT library for linking
if (WIN32)
    set(LUAJIT_LIB "${LUAJIT_SRC_DIR}/lua51.lib")
elseif (APPLE)
    set(LUAJIT_LIB "${LUAJIT_SRC_DIR}/libluajit.a")
endif()

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
