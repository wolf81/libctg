#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ctg_grid.h"
#include "ctg_movestack.h"

ErrorCode last_error = SUCCESS;

Grid* ctg_grid_create(int width, int height, int* values) {
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
        ctg_grid_destroy(grid);
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

    ctg_movestack_init(&grid->moveHistory, 100);
    return grid;
}

void ctg_grid_destroy(Grid* grid) {
    if (grid) {
        for (int y = 0; y < grid->height; y++) {
            free(grid->values[y]);
        }
        free(grid->values);
        grid->values = NULL;

        ctg_movestack_free(&grid->moveHistory);
        free(grid);
    }
}

bool ctg_grid_is_solved(const Grid* grid) {
    return grid->score == 0;
}

int ctg_grid_get_value(const Grid* grid, int x, int y) {
    if (!ctg_grid_in_bounds(grid, x, y)) return -1;
    return grid->values[y][x];
}

bool ctg_grid_in_bounds(const Grid* grid, int x, int y) {
    return x >= 0 && x < grid->width && y >= 0 && y < grid->height;
}

