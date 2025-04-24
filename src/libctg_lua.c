#include <lua.h>
#include <lauxlib.h>
#include "libctg.h"

#define GRID_MT "libctg.grid_mt" // Grid metatable

static int l_parseGrid(lua_State* L) {
    const char* input = luaL_checkstring(L, 1);

    Grid* grid = parseGrid(input);

    if (!grid) {
        lua_pushnil(L);
        lua_pushstring(L, "Failed to parse grid");        
        return 2;
    }

    // Create a new table and set it as a Grid object
    lua_newtable(L);
    lua_pushlightuserdata(L, grid);  // Push the Grid pointer as a light userdata
    lua_setfield(L, -2, "grid_ptr");  // Set the Grid pointer in the table

    // Set the metatable for the Grid object
    luaL_getmetatable(L, GRID_MT);
    lua_setmetatable(L, -2);

    return 1;  // Return the Grid object
}

static int l_printGrid(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);  // Ensure the argument is a table (Grid object)

    // Get the Grid pointer from the table
    lua_getfield(L, 1, "grid_ptr");
    Grid* grid = (Grid*)lua_touserdata(L, -1);

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }

    // Call the C function to print the grid
    printGrid(grid);

    return 0;  // No return value
}

static int l_getValue(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);  // Ensure the argument is a table (Grid object)
    int x = (int)luaL_checkinteger(L, 2) - 1; // Get x coordinate
    int y = (int)luaL_checkinteger(L, 3) - 1; // Get y coordinate

    // Get the Grid pointer from the table
    lua_getfield(L, 1, "grid_ptr");
    Grid* grid = (Grid*)lua_touserdata(L, -1);

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }

    int value = getValue(grid, x, y);
    lua_pushinteger(L, value);

    return 1;
}

static int l_isValidMove(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);  // Ensure the argument is a table (Grid object)
    int x = (int)luaL_checkinteger(L, 2) - 1; // Get x coordinate
    int y = (int)luaL_checkinteger(L, 3) - 1; // Get y coordinate
    const char *dir_str = luaL_checkstring(L, 4);  // Get direction string ('U', 'D', 'L', 'R')

    // Get the Grid pointer from the table
    lua_getfield(L, 1, "grid_ptr");
    Grid* grid = (Grid*)lua_touserdata(L, -1);

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
    luaL_checktype(L, 1, LUA_TTABLE);  // Ensure the argument is a table (Grid object)

    // Get the Grid pointer from the table
    lua_getfield(L, 1, "grid_ptr");
    Grid* grid = (Grid*)lua_touserdata(L, -1);

    if (grid != NULL) {
        // Call the C function to free the grid
        freeGrid(grid);
    }

    return 0;  // No return value
}

static int l_getSize(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);  // Ensure the argument is a table (Grid object)

    // Get the Grid pointer from the table
    lua_getfield(L, 1, "grid_ptr");
    Grid* grid = (Grid*)lua_touserdata(L, -1);

    if (grid == NULL) {
        lua_pushstring(L, "Invalid Grid object");
        return 1;
    }

    lua_pushinteger(L, grid->width);
    lua_pushinteger(L, grid->height);
    return 2;
}

static void createGridMetatable(lua_State* L) {
    if (luaL_newmetatable(L, GRID_MT)) {
        // Set __gc to freeGrid
        lua_pushcfunction(L, l_freeGrid);
        lua_setfield(L, -2, "__gc");

        // Create method table
        lua_newtable(L);

        lua_pushcfunction(L, l_printGrid);
        lua_setfield(L, -2, "print");

        lua_pushcfunction(L, l_isValidMove);
        lua_setfield(L, -2, "isValidMove");

        lua_pushcfunction(L, l_getValue);
        lua_setfield(L, -2, "getValue");

        lua_pushcfunction(L, l_getSize);
        lua_setfield(L, -2, "getSize");

        // Assign method table to __index
        lua_setfield(L, -2, "__index");
    }

    lua_pop(L, 1);  // pop metatable
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
