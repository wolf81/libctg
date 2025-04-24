#include <lua.h>
#include <lauxlib.h>
#include "libctg.h"

#define GRID_MT "libctg.grid_mt"  // Unique name for metatable

static int l_parseGrid(lua_State* L) {
    const char* input = luaL_checkstring(L, 1);
    Grid* grid = parseGrid(input);
    if (!grid) {
        return luaL_error(L, "Failed to parse grid");
    }

    // Create userdata and attach metatable
    Grid** udata = (Grid**)lua_newuserdata(L, sizeof(Grid*));
    *udata = grid;

    luaL_getmetatable(L, GRID_MT);
    lua_setmetatable(L, -2);
    return 1;
}

static int l_gridPrint(lua_State* L) {
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);
    printGrid(grid);
    return 0;
}

static int l_gridGC(lua_State* L) {
    Grid* grid = *(Grid**)luaL_checkudata(L, 1, GRID_MT);
    freeGrid(grid);
    return 0;
}

static void createGridMetatable(lua_State* L) {
    luaL_newmetatable(L, GRID_MT);

    lua_pushcfunction(L, l_gridGC);
    lua_setfield(L, -2, "__gc");

    lua_newtable(L);  // Method table

    lua_pushcfunction(L, l_gridPrint);
    lua_setfield(L, -2, "print");

    lua_setfield(L, -2, "__index");  // metatable.__index = method table
    lua_pop(L, 1); // pop metatable
}

static int l_getLastError(lua_State* L) {
    lua_pushinteger(L, last_error);
    return 1;
}

LIBCTG_API int luaopen_libctg(lua_State* L) {
    createGridMetatable(L);

    luaL_Reg funcs[] = {
        {"parseGrid", l_parseGrid},
        {"getLastError", l_getLastError},
        {NULL, NULL}
    };
    luaL_newlib(L, funcs);
    return 1;
}
