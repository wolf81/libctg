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

-- grid returns light user data
local grid = ctg.parseGrid(input) --> table: 0000000000A73540

-- use print to view grid layout
grid:print()    --> 0 0 0 0 0 0 0 0
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

-- get the values in the grid, row by row and column by column
local s = ''
for _, row in ipairs(grid:getValues()) do
    s = s .. table.concat(row, ',') .. '\n'
end
print(s)    --> 0,0,0,0,0,0,0,0,
            --> 7,0,0,0,0,0,0,7,
            --> 3,1,2,0,0,0,0,0,
            --> 0,0,0,0,0,0,0,0,
            --> 0,0,0,0,0,0,1,7,
```
