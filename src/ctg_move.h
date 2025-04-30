#ifndef CGG_MOVE_H
#define CGG_MOVE_H

#include "ctg_types.h"
#include "ctg_grid.h"

typedef struct {
    int x;
    int y;
    int value;
    int change;
} MoveResult;

LIBCTG_API bool validateGridMove(const Grid* grid, Move* move);

LIBCTG_API MoveResult executeGridMove(Grid* grid, Move* move);

LIBCTG_API MoveResult peekGridMove(const Grid* grid, Move* move);

LIBCTG_API bool revertGridMove(Grid* grid);

#endif
