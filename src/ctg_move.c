#include <stdlib.h>
#include "ctg_move.h"
#include "ctg_movestack.h"

bool ctg_move_validate(const Grid* grid, Move* move) {
    if (!ctg_grid_in_bounds(grid, move->x, move->y)) return false;
    int value = grid->values[move->y][move->x];
    if (value == 0) return false;

    int tx = move->x + move->dir.dx * value;
    int ty = move->y + move->dir.dy * value;
    if (!ctg_grid_in_bounds(grid, tx, ty)) return false;
    if (grid->values[ty][tx] == 0) return false;

    return true;
}

MoveResult ctg_move_execute(Grid* grid, Move* move) {
    if (!ctg_move_validate(grid, move)) {
        return (MoveResult){ -1, -1, 0, 0 };
    }

    if (grid->moveHistory.size == grid->moveHistory.capacity) {
        ctg_movestack_resize(&grid->moveHistory);
    }

    int sx = move->x, sy = move->y;
    int value = grid->values[sy][sx];
    int tx = sx + move->dir.dx * value;
    int ty = sy + move->dir.dy * value;

    int tvalue = grid->values[ty][tx];
    int change = move->add ? value : -value;

    grid->moveHistory.moves[grid->moveHistory.size++] = (MoveRecord){ *move, value, tvalue };

    grid->values[sy][sx] = 0;
    grid->values[ty][tx] = abs(tvalue + change);
    grid->score -= (value + tvalue);
    grid->score += grid->values[ty][tx];

    return (MoveResult){ tx, ty, grid->values[ty][tx], change };
}

MoveResult ctg_move_peek(const Grid* grid, Move* move) {
    if (!ctg_move_validate(grid, move)) {
        return (MoveResult){ -1, -1, 0, 0 };
    }

    int sx = move->x, sy = move->y;
    int svalue = grid->values[sy][sx];
    int tx = sx + move->dir.dx * svalue;
    int ty = sy + move->dir.dy * svalue;
    int tvalue = grid->values[ty][tx];
    int change = move->add ? svalue : -svalue;

    return (MoveResult){ tx, ty, abs(tvalue + change), change };
}

bool ctg_move_revert(Grid* grid) {
    if (grid->moveHistory.size == 0) return false;

    MoveRecord record = grid->moveHistory.moves[--grid->moveHistory.size];
    Move move = record.move;

    int sx = move.x;
    int sy = move.y;

    int tx = sx + move.dir.dx * record.svalue;
    int ty = sy + move.dir.dy * record.svalue;

    grid->score -= grid->values[ty][tx];
    grid->score += (record.svalue + record.tvalue);

    grid->values[sy][sx] = record.svalue;
    grid->values[ty][tx] = record.tvalue;

    return true;
}
