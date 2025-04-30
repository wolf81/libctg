#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include "ctg.h"

#define GRID_MT "ctg.grid_mt" // Grid metatable

// Function to convert the direction enum to a string.
static const char* directionToString(Direction dir) {
    // Handle horizontal directions (Left/Right)
    if (dir.dy == 0) {
        if (dir.dx == 1) return "R";  // Right
        if (dir.dx == -1) return "L"; // Left
    }

    // Handle vertical directions (Up/Down)
    if (dir.dx == 0) {
        if (dir.dy == 1) return "D";  // Up
        if (dir.dy == -1) return "U"; // Down
    }

    // If the direction doesn't match any valid case, return NULL
    return NULL;
}

static int l_gridFromString(lua_State* L) {
    const char* input = luaL_checkstring(L, 1);

    // Parse the grid from the input string
    Grid* grid = gridFromString(input);

    if (!grid) {
        lua_pushnil(L);  // Return nil to indicate failure
        lua_pushstring(L, "Failed to parse grid");
        return 2;
    }

    // Create a new userdata to hold the Grid object
    Grid** grid_ptr = (Grid**)lua_newuserdata(L, sizeof(Grid*));  // Create userdata for Grid pointer
    *grid_ptr = grid;  // Set the Grid pointer in the userdata

    // Set the metatable for the userdata (this ensures the __tostring metamethod is available)
    luaL_getmetatable(L, GRID_MT);  // Get the metatable for the Grid
    lua_setmetatable(L, -2);        // Set the metatable for the userdata

    return 1;  // Return the userdata (Grid object) to Lua
}

static int l_gridToString(lua_State* L) {
    // Ensure the argument is a userdata (Grid object)
    luaL_checktype(L, 1, LUA_TUSERDATA);

    // Get the Grid pointer from the userdata
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);  // Use the correct metatable

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }

    char* s = gridToString(grid);  // Convert Grid to string using toString function
    lua_pushstring(L, s);
    free(s);  // Free the string returned by toString

    return 1;  // Return the string to Lua
}

static int l_getGridValue(lua_State* L) {
    // Ensure the argument is a userdata (Grid object)
    luaL_checktype(L, 1, LUA_TUSERDATA);

    int x = (int)luaL_checkinteger(L, 2) - 1; // Get x coordinate
    int y = (int)luaL_checkinteger(L, 3) - 1; // Get y coordinate

    // Get the Grid pointer from the userdata
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);  // Use the correct metatable

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }

    int value = getGridValue(grid, x, y);
    lua_pushinteger(L, value);

    return 1;
}

static int l_isGridSolved(lua_State* L) {
    // Ensure the argument is a userdata (Grid object)
    luaL_checktype(L, 1, LUA_TUSERDATA);

    // Get the Grid pointer from the userdata
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);  // Use the correct metatable

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }

    if (isGridSolved(grid)) {
        lua_pushboolean(L, 1);  // Return true if the move is valid
    } else {
        lua_pushboolean(L, 0);  // Return false if the move is invalid
    }

    return 1;
}

static int l_peekGridMove(lua_State* L) {
    // Ensure the argument is a userdata (Grid object)
    luaL_checktype(L, 1, LUA_TUSERDATA);

    int x = (int)luaL_checkinteger(L, 2) - 1; // Get x coordinate
    int y = (int)luaL_checkinteger(L, 3) - 1; // Get y coordinate
    const char *dir_str = luaL_checkstring(L, 4);  // Get direction string ('U', 'D', 'L', 'R')

    luaL_checktype(L, 5, LUA_TBOOLEAN);  // ensure it's a boolean
    int add = lua_toboolean(L, 5);

    // Get the Grid pointer from the userdata
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);  // Use the correct metatable

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }
    
    // Convert direction string to Direction struct
    Direction dir = {0, 0};
    if (dir_str != NULL) {
        if (dir_str[0] == 'U') {
            dir.dy = -1;
        } else if (dir_str[0] == 'D') {
            dir.dy = 1;
        } else if (dir_str[0] == 'L') {
            dir.dx = -1;
        } else if (dir_str[0] == 'R') {
            dir.dx = 1;
        } else {
            lua_pushstring(L, "Invalid direction");
            return 1;
        }
    }
    // Create a Move struct and fill it
    Move move = {x, y, dir, add};

    MoveResult result = peekGridMove(grid, &move);
    lua_pushinteger(L, result.x + 1);
    lua_pushinteger(L, result.y + 1);
    lua_pushinteger(L, result.value);
    lua_pushinteger(L, result.change);
    
    return 4;    
}

static int l_revertGridMove(lua_State* L) {
    // Ensure the argument is a userdata (Grid object)
    luaL_checktype(L, 1, LUA_TUSERDATA);

    // Get the Grid pointer from the userdata
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);  // Use the correct metatable

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }

    // Call the C function to check if the move is valid
    if (revertGridMove(grid)) {
        lua_pushboolean(L, 1);  // Return true if the move is valid
    } else {
        lua_pushboolean(L, 0);  // Return false if the move is invalid
    }

    return 1;
}

static int l_executeGridMove(lua_State* L) {
    // Ensure the argument is a userdata (Grid object)
    luaL_checktype(L, 1, LUA_TUSERDATA);

    int x = (int)luaL_checkinteger(L, 2) - 1; // Get x coordinate
    int y = (int)luaL_checkinteger(L, 3) - 1; // Get y coordinate
    const char *dir_str = luaL_checkstring(L, 4);  // Get direction string ('U', 'D', 'L', 'R')

    luaL_checktype(L, 5, LUA_TBOOLEAN);  // ensure it's a boolean
    int add = lua_toboolean(L, 5);

    // Get the Grid pointer from the userdata
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);  // Use the correct metatable

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }
    
    // Convert direction string to Direction struct
    Direction dir = {0, 0};
    if (dir_str != NULL) {
        if (dir_str[0] == 'U') {
            dir.dy = -1;
        } else if (dir_str[0] == 'D') {
            dir.dy = 1;
        } else if (dir_str[0] == 'L') {
            dir.dx = -1;
        } else if (dir_str[0] == 'R') {
            dir.dx = 1;
        } else {
            lua_pushstring(L, "Invalid direction");
            return 1;
        }
    }
    // Create a Move struct and fill it
    Move move = {x, y, dir, add};

    MoveResult result = executeGridMove(grid, &move);
    lua_pushinteger(L, result.x + 1);
    lua_pushinteger(L, result.y + 1);
    lua_pushinteger(L, result.value);
    lua_pushinteger(L, result.change);
    
    return 4;    
}

static int l_validateGridMove(lua_State* L) {
    // Ensure the argument is a userdata (Grid object)
    luaL_checktype(L, 1, LUA_TUSERDATA);

    int x = (int)luaL_checkinteger(L, 2) - 1; // Get x coordinate
    int y = (int)luaL_checkinteger(L, 3) - 1; // Get y coordinate
    const char *dir_str = luaL_checkstring(L, 4);  // Get direction string ('U', 'D', 'L', 'R')

    // Get the Grid pointer from the userdata
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);  // Use the correct metatable

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }

    // Convert direction string to Direction struct
    Direction dir = {0, 0};
    if (dir_str != NULL) {
        if (dir_str[0] == 'U') {
            dir.dy = -1;
        } else if (dir_str[0] == 'D') {
            dir.dy = 1;
        } else if (dir_str[0] == 'L') {
            dir.dx = -1;
        } else if (dir_str[0] == 'R') {
            dir.dx = 1;
        } else {
            lua_pushstring(L, "Invalid direction");
            return 1;
        }
    }

    // Create a Move struct and fill it
    Move move = {x, y, dir, true};

    // Call the C function to check if the move is valid
    if (validateGridMove(grid, &move)) {
        lua_pushboolean(L, 1);  // Return true if the move is valid
    } else {
        lua_pushboolean(L, 0);  // Return false if the move is invalid
    }

    return 1;  // Return the boolean result
}

static int l_destroyGrid(lua_State* L) {
    // Ensure the argument is a userdata (Grid object)
    luaL_checktype(L, 1, LUA_TUSERDATA);

    // Get the Grid pointer from the userdata
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);  // Use the correct metatable

    if (grid != NULL) {
        // Call the C function to free the grid
        destroyGrid(grid);
    }

    return 0;  // No return value
}

static int l_getGridSize(lua_State* L) {
    // Ensure the argument is a userdata (Grid object)
    luaL_checktype(L, 1, LUA_TUSERDATA);

    // Get the Grid pointer from the userdata
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);  // Use the correct metatable

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }

    lua_pushinteger(L, grid->width);
    lua_pushinteger(L, grid->height);
    return 2;
}

static int l_getMoveHistory(lua_State* L) {
    // Ensure the argument is a userdata (Grid object)
    luaL_checktype(L, 1, LUA_TUSERDATA);

    // Get the Grid pointer from the userdata
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);  // Use the correct metatable

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }

    lua_newtable(L);  // Create a new table for the result.

    for (int i = 0; i < grid->moveHistory.size; ++i) {
        Move* move = &grid->moveHistory.moves[i].move;

        // Create a table for each move
        lua_pushinteger(L, i + 1);  // Set the key (index) for the move.
        lua_newtable(L);  // Create a new table for this move.

        // Add x and y coordinates
        lua_pushinteger(L, move->x + 1);
        lua_rawseti(L, -2, 1);

        lua_pushinteger(L, move->y + 1);
        lua_rawseti(L, -2, 2);

        // Add the direction as a string
        lua_pushstring(L, directionToString(move->dir));
        lua_rawseti(L, -2, 3);

        // Add the add flag (true/false)
        lua_pushboolean(L, move->add);
        lua_rawseti(L, -2, 4);

        // Set the move table in the result table
        lua_settable(L, -3);
    }

    return 1;  // Return the move history table.    
}

static int l_gridPairsIter(lua_State* L) {
    GridIterator* iter = (GridIterator*)lua_touserdata(L, lua_upvalueindex(1));

    int x, y, value;
    if (!gridIteratorNext(iter, &x, &y, &value)) {
        return 0; // end of iteration
    }

    lua_pushinteger(L, x + 1);
    lua_pushinteger(L, y + 1);
    lua_pushinteger(L, value);
    return 3;
}

static int l_gridPairs(lua_State* L) {
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);

    // Allocate userdata for GridIterator
    GridIterator* iter = (GridIterator*)lua_newuserdata(L, sizeof(GridIterator));
    initGridIterator(iter, grid); // Initialize the iterator

    // Push the iterator function with 1 upvalue (GridIterator*)
    lua_pushcclosure(L, l_gridPairsIter, 1);
    return 1;
}

static void createGridMetatable(lua_State* L) {
    // Create the metatable for the grid (returns 1 if the metatable is new)
    if (luaL_newmetatable(L, GRID_MT)) {
        // assign meta-methods
        lua_pushcfunction(L, l_destroyGrid);
        lua_setfield(L, -2, "__gc"); // automatic garbage collection

        lua_pushcfunction(L, l_gridToString);  // Push the toString function
        lua_setfield(L, -2, "__tostring"); // Assign it as the __tostring metamethod

        // Create method table (a table to store methods)
        lua_newtable(L);  // Create a new table to store methods

        lua_pushcfunction(L, l_gridPairs);
        lua_setfield(L, -2, "iter");

        lua_pushcfunction(L, l_validateGridMove);
        lua_setfield(L, -2, "isValidMove");

        lua_pushcfunction(L, l_executeGridMove);
        lua_setfield(L, -2, "applyMove");

        lua_pushcfunction(L, l_revertGridMove);
        lua_setfield(L, -2, "revertMove");

        lua_pushcfunction(L, l_peekGridMove);
        lua_setfield(L, -2, "peekMove");

        lua_pushcfunction(L, l_getGridValue);
        lua_setfield(L, -2, "getValue");

        lua_pushcfunction(L, l_isGridSolved);
        lua_setfield(L, -2, "isSolved");

        lua_pushcfunction(L, l_getGridSize);
        lua_setfield(L, -2, "getSize");

        lua_pushcfunction(L, l_getMoveHistory);
        lua_setfield(L, -2, "getMoves");

        // Assign the method table to the __index field of the metatable
        lua_setfield(L, -2, "__index");

        // Pop the method table, we no longer need it on the stack
        lua_pop(L, 1);
    }

    // At this point, the metatable is on the stack.
    // We can now pop it from the stack (clean up).
    lua_pop(L, 1);
}

static int l_getLastError(lua_State* L) {
    lua_pushinteger(L, last_error);
    return 1;
}

LIBCTG_API int luaopen_libctg(lua_State* L) {
      // Create and register the metatable
    createGridMetatable(L);  // use your existing helper

    // Create the module table
    lua_newtable(L);

    lua_pushcfunction(L, l_gridFromString);
    lua_setfield(L, -2, "parseGrid");

    lua_pushcfunction(L, l_getLastError);
    lua_setfield(L, -2, "getLastError");

    return 1;  // Return the module
}
