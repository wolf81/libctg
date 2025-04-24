#!/bin/sh

echo "Building LuaJIT ..."
cd deps/luajit
cmake .
cmake --build .
