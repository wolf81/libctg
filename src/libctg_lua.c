#include <lua.h>
#include <lauxlib.h>
#include <stdlib.h>
#include "libctg.h"

#define GRID_MT "libctg.grid_mt" // Grid metatable

static int l_parseGrid(lua_State* L) {
    const char* input = luaL_checkstring(L, 1);

    // Parse the grid from the input string
    Grid* grid = parseGrid(input);

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

static int l_toString(lua_State* L) {
    // Ensure the argument is a userdata (Grid object)
    luaL_checktype(L, 1, LUA_TUSERDATA);

    // Get the Grid pointer from the userdata
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);  // Use the correct metatable

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }

    char* s = toString(grid);  // Convert Grid to string using toString function
    lua_pushstring(L, s);
    free(s);  // Free the string returned by toString

    return 1;  // Return the string to Lua
}

static int l_getValue(lua_State* L) {
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

    int value = getValue(grid, x, y);
    lua_pushinteger(L, value);

    return 1;
}

static int l_isSolved(lua_State* L) {
    // Ensure the argument is a userdata (Grid object)
    luaL_checktype(L, 1, LUA_TUSERDATA);

    // Get the Grid pointer from the userdata
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);  // Use the correct metatable

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }

    if (isSolved(grid)) {
        lua_pushboolean(L, 1);  // Return true if the move is valid
    } else {
        lua_pushboolean(L, 0);  // Return false if the move is invalid
    }

    return 1;
}

static int l_applyMove(lua_State* L) {
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

    MoveResult result = playMove(grid, &move);
    lua_pushinteger(L, result.x + 1);
    lua_pushinteger(L, result.y + 1);
    lua_pushinteger(L, result.value);
    
    return 3;    
}

static int l_isValidMove(lua_State* L) {
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
    if (isValidMove(grid, &move)) {
        lua_pushboolean(L, 1);  // Return true if the move is valid
    } else {
        lua_pushboolean(L, 0);  // Return false if the move is invalid
    }

    return 1;  // Return the boolean result
}

static int l_freeGrid(lua_State* L) {
    // Ensure the argument is a userdata (Grid object)
    luaL_checktype(L, 1, LUA_TUSERDATA);

    // Get the Grid pointer from the userdata
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);  // Use the correct metatable

    if (grid != NULL) {
        // Call the C function to free the grid
        freeGrid(grid);
    }

    return 0;  // No return value
}

static int l_getSize(lua_State* L) {
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

static int l_pairsIter(lua_State* L) {
    Grid* grid = (Grid*)lua_touserdata(L, lua_upvalueindex(1));
    int index = lua_tointeger(L, lua_upvalueindex(2));

    if (index >= grid->length) {
        return 0; // end iteration
    }

    int x = (index % grid->width) + 1;
    int y = (index / grid->width) + 1;
    int value = grid->values[index];

    // Update index for next call
    lua_pushinteger(L, index + 1);
    lua_replace(L, lua_upvalueindex(2));

    lua_pushinteger(L, x);
    lua_pushinteger(L, y);
    lua_pushinteger(L, value);
    return 3;
}

// The __pairs metamethod
static int l_pairs(lua_State* L) {
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);

    lua_pushlightuserdata(L, grid);  // upvalue 1
    lua_pushinteger(L, 0);           // upvalue 2 (start index)
    lua_pushcclosure(L, l_pairsIter, 2);
    return 1; // returns the closure
}

static void createGridMetatable(lua_State* L) {
    // Create the metatable for the grid (returns 1 if the metatable is new)
    if (luaL_newmetatable(L, GRID_MT)) {
        // assign meta-methods
        lua_pushcfunction(L, l_freeGrid);
        lua_setfield(L, -2, "__gc"); // automatic garbage collection

        lua_pushcfunction(L, l_toString);  // Push the toString function
        lua_setfield(L, -2, "__tostring"); // Assign it as the __tostring metamethod

        // Create method table (a table to store methods)
        lua_newtable(L);  // Create a new table to store methods

        lua_pushcfunction(L, l_pairs);
        lua_setfield(L, -2, "iter");

        lua_pushcfunction(L, l_isValidMove);
        lua_setfield(L, -2, "isValidMove");

        lua_pushcfunction(L, l_applyMove);
        lua_setfield(L, -2, "applyMove");

        lua_pushcfunction(L, l_getValue);
        lua_setfield(L, -2, "getValue");

        lua_pushcfunction(L, l_isSolved);
        lua_setfield(L, -2, "isSolved");

        lua_pushcfunction(L, l_getSize);
        lua_setfield(L, -2, "getSize");

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

    lua_pushcfunction(L, l_parseGrid);
    lua_setfield(L, -2, "parseGrid");

    lua_pushcfunction(L, l_getLastError);
    lua_setfield(L, -2, "getLastError");

    return 1;  // Return the module
}
