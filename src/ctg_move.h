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

LIBCTG_API bool ctg_move_validate(const Grid* grid, Move* move);

LIBCTG_API MoveResult ctg_move_execute(Grid* grid, Move* move);

LIBCTG_API MoveResult ctg_move_peek(const Grid* grid, Move* move);

LIBCTG_API bool ctg_move_revert(Grid* grid);

#endif
