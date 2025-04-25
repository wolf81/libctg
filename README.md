# libctg

A C library to calculate solutions for Clear the Grid.

# Usage

The library can be used either from C or Lua. PLEASE NOTE: When using from Lua, the library is only compatible with Lua 5.1 as this is the ABI compatability of LuaJIT in LÖVE.

So, when trying to run using a Lua interpreter, ensure you're using Lua 5.1. You can verify the version with the command `$ lua -v`. Alternatively, install a LuaJIT interpreter or just use LÖVE.

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

-- create a grid object by parsing a grid string
local grid = ctg.parseGrid(input)

-- use Lua's print() function to view the grid layout
print(grid) --> 0 0 0 0 0 0 0 0
            --> 7 0 0 0 0 0 0 7
            --> 3 1 2 0 0 0 0 0
            --> 0 0 0 0 0 0 0 0
            --> 0 0 0 0 0 0 0 7

-- check the value at a coordinate, will return -1 if out of bounds
local value = grid:getValue(8, 5) --> 7

-- check the size of the grid
local w, h = grid:getSize() --> 8, 5

-- check if a move is possible from a coord, with a direction
local can_move = grid:isValidMove(8, 5, 'R') --> false

-- check if a grid is solved, which is true if all values are 0
local is_solved = grid:isSolved() --> false

-- iterate over the values in the grid
for x, y, value in ipairs(grid:iter()) do
    print(string.format('%d, %d: %d', x, y, value)) --> 1, 1: 0 ... 8, 5: 7
end
```
