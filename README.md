# libctg

A C library to calculate solutions for Clear the Grid.

# Usage

The library can be used either from C or Lua.

## Lua

In order to use the library from Lua, ensure the DLL is in the root directory of the project. And then the library can be used as such:

```lua
local ctg = require 'libctg'

local input = [[
    8 5
    0 0 0 0 0 0 0 0
    7 0 0 0 0 0 0 7
    3 1 2 0 0 0 0 0
    0 0 0 0 0 0 0 0
    0 0 0 0 0 0 0 7
]]
    
local grid = ctg.parseGrid(input)

grid:print() --> 0 0 0 0 0 0 0 0
			 --> 7 0 0 0 0 0 0 7
			 --> 3 1 2 0 0 0 0 0
			 --> 0 0 0 0 0 0 0 0
			 --> 0 0 0 0 0 0 0 7

grid:getValue(8, 5) --> 7

grid:isValidMove(8, 5, 'R', false) --> false
```
