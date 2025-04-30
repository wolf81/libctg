#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctg_grid.h"
#include "ctg_movestack.h"

ErrorCode last_error = SUCCESS;

void initMoveStack(MoveStack* stack, int capacity) {
    stack->moves = malloc(sizeof(MoveRecord) * capacity);
    stack->size = 0;
    stack->capacity = capacity;
}

void freeMoveStack(MoveStack* stack) {
    if (stack) {
        free(stack->moves);
        stack->moves = NULL;
        stack->size = stack->capacity = 0;
    }
}

bool resizeMoveStack(MoveStack* stack) {
    int new_capacity = stack->capacity * 2;
    MoveRecord* new_moves = realloc(stack->moves, sizeof(MoveRecord) * new_capacity);
    if (new_moves == NULL) {
        last_error = ERR_MEMORY_ALLOCATION;
        return false;
    }
    stack->moves = new_moves;
    stack->capacity = new_capacity;
    return true;
}

Grid* initGrid(int width, int height, int* values) {
    Grid* grid = (Grid*)malloc(sizeof(Grid));
    if (grid == NULL) {
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    grid->width = width;
    grid->height = height;
    grid->length = width * height;
    grid->score = 0;

    grid->values = malloc(height * sizeof(int*));
    if (!grid->values) {
        destroyGrid(grid);
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    for (int y = 0; y < height; y++) {
        grid->values[y] = malloc(width * sizeof(int));
        if (!grid->values[y]) {
            for (int j = 0; j < y; j++) free(grid->values[j]);
            free(grid->values);
            free(grid);
            last_error = ERR_MEMORY_ALLOCATION;
            return NULL;
        }
        for (int x = 0; x < width; x++) {
            int i = y * width + x;
            grid->values[y][x] = values[i];
            grid->score += values[i];
        }
    }

    initMoveStack(&grid->moveHistory, 100);
    return grid;
}

void destroyGrid(Grid* grid) {
    if (grid) {
        for (int y = 0; y < grid->height; y++) {
            free(grid->values[y]);
        }
        free(grid->values);
        grid->values = NULL;

        freeMoveStack(&grid->moveHistory);
        free(grid);
    }
}

bool isGridSolved(const Grid* grid) {
    return grid->score == 0;
}

int getGridValue(const Grid* grid, int x, int y) {
    if (!inBounds(grid, x, y)) return -1;
    return grid->values[y][x];
}

bool inBounds(const Grid* grid, int x, int y) {
    return x >= 0 && x < grid->width && y >= 0 && y < grid->height;
}

