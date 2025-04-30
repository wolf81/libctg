#ifndef CTG_GRID_H
#define CTG_GRID_H

#include "ctg_types.h"

LIBCTG_API ErrorCode last_error;

LIBCTG_API Grid* ctg_grid_create(int width, int height, int* values);

LIBCTG_API void ctg_grid_destroy(Grid* grid);

LIBCTG_API int ctg_grid_get_value(const Grid* grid, int x, int y);

LIBCTG_API bool ctg_grid_is_solved(const Grid* grid);

LIBCTG_API bool ctg_grid_in_bounds(const Grid* grid, int x, int y);

#endif
