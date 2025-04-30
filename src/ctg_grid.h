#ifndef CTG_GRID_H
#define CTG_GRID_H

#include "ctg_types.h"

LIBCTG_API ErrorCode last_error;

LIBCTG_API Grid* initGrid(int width, int height, int* values);

LIBCTG_API void destroyGrid(Grid* grid);

LIBCTG_API int getGridValue(const Grid* grid, int x, int y);

LIBCTG_API bool isGridSolved(const Grid* grid);

LIBCTG_API bool inBounds(const Grid* grid, int x, int y);

#endif
