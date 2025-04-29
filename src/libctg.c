#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libctg.h"
#include "utils.c"

ErrorCode last_error = SUCCESS;

void initGridIterator(GridIterator* iter, const Grid* grid) {
    iter->grid = grid;
    iter->index = 0;
}

bool gridIteratorNext(GridIterator* iter, int* out_x, int* out_y, int* out_value) {
    if (iter->index >= iter->grid->length) {
        return false; // Finished
    }
    *out_x = iter->index % iter->grid->width;
    *out_y = iter->index / iter->grid->width;
    *out_value = iter->grid->values[*out_y][*out_x];

    iter->index++;
    return true;
}

void initMoveStack(MoveStack* stack, int capacity) {
    stack->moves = malloc(sizeof(Move) * capacity);
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
    Move* new_moves = realloc(stack->moves, sizeof(Move) * new_capacity);
    if (new_moves == NULL) {
        last_error = ERR_MEMORY_ALLOCATION;
        return false;
    }
    stack->moves = new_moves;
    stack->capacity = new_capacity;
    return true;
}

Grid* initGrid(int width, int height, int* flatValues) {
    Grid* grid = (Grid*)malloc(sizeof(Grid));
    if (!grid) {
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    grid->width = width;
    grid->height = height;
    grid->length = width * height;
    grid->score = 0;

    grid->values = (int**)malloc(height * sizeof(int*));
    if (!grid->values) {
        free(grid);
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    for (int y = 0; y < height; ++y) {
        grid->values[y] = (int*)malloc(width * sizeof(int));
        if (!grid->values[y]) {
            for (int k = 0; k < y; ++k) free(grid->values[k]);
            free(grid->values);
            free(grid);
            last_error = ERR_MEMORY_ALLOCATION;
            return NULL;
        }
        for (int x = 0; x < width; ++x) {
            int value = flatValues[y * width + x];
            grid->values[y][x] = value;
            grid->score += value;
        }
    }

    initMoveStack(&grid->moveHistory, 100);
    return grid;
}

void destroyGrid(Grid* grid) {
    if (grid) {
        if (grid->values) {
            for (int y = 0; y < grid->height; ++y) {
                free(grid->values[y]);
            }
            free(grid->values);
            grid->values = NULL;
        }

        freeMoveStack(&grid->moveHistory);
        free(grid);
    }
}

char* gridToString(const Grid* grid) {
    int estimatedSize = grid->length * 4 + grid->height + 1;
    char* buffer = malloc(estimatedSize);
    if (!buffer) return NULL;

    char* ptr = buffer;
    int remaining = estimatedSize;

    for (int y = 0; y < grid->height; ++y) {
        for (int x = 0; x < grid->width; ++x) {
            int written = snprintf(ptr, remaining, "%d ", grid->values[y][x]);
            ptr += written;
            remaining -= written;
        }
        *ptr++ = '\n';
        remaining--;
    }

    *ptr = '\0';
    return buffer;
}

Grid* gridFromString(const char* input) {
    input = strcat(trim((char*)input), "\n");
    int width = 0, height = 0;
    if (sscanf(input, "%d %d\n", &width, &height) != 2) {
        last_error = ERR_INVALID_INPUT;
        return NULL;
    }

    int length = width * height;
    int *values = (int *)malloc(sizeof(int) * length);
    if (values == NULL) {
        last_error = ERR_MEMORY_ALLOCATION;
        return NULL;
    }

    const char *ptr = strchr(input, '\n') + 1;
    int i = 0;
    char line[256];

    while (*ptr != '\0') {
        char *line_end = strchr(ptr, '\n');
        size_t line_len = (line_end) ? (line_end - ptr) : strlen(ptr);
        strncpy(line, ptr, line_len);
        line[line_len] = '\0';

        char *trimmed = trim(line);
        char *token = strtok(trimmed, " ");
        while (token) {
            values[i++] = atoi(token);
            token = strtok(NULL, " ");
        }

        ptr = line_end ? (line_end + 1) : NULL;
    }

    if (i != length) {
        last_error = ERR_INVALID_INPUT;
        return NULL;
    }

    Grid *grid = initGrid(width, height, values);
    free(values);
    return grid;
}

static bool inBounds(const Grid* grid, int x, int y) {
    return x >= 0 && x < grid->width && y >= 0 && y < grid->height;
}

bool validateGridMove(const Grid* grid, Move* move) {
    if (!inBounds(grid, move->x, move->y)) {
        return false;
    }

    int value = grid->values[move->y][move->x];
    if (value == 0) {
        return false;
    }

    int tx = move->x + move->dir.dx * value;
    int ty = move->y + move->dir.dy * value;
    if (!inBounds(grid, tx, ty)) {
        return false;
    }

    if (grid->values[ty][tx] == 0) {
        return false;
    }

    return true;
}

MoveResult executeGridMove(Grid* grid, Move* move) {
    if (!validateGridMove(grid, move)) {
        return (MoveResult){ -1, -1, 0, 0 };
    }

    if (grid->moveHistory.size == grid->moveHistory.capacity) {
        resizeMoveStack(&grid->moveHistory);
    }

    int value = grid->values[move->y][move->x];
    int tx = move->x + move->dir.dx * value;
    int ty = move->y + move->dir.dy * value;
    int tvalue = grid->values[ty][tx];

    grid->moveHistory.moves[grid->moveHistory.size++] = *move;
    int change = move->add ? value : -value;

    grid->values[move->y][move->x] = 0;
    grid->values[ty][tx] = abs(tvalue + change);

    grid->score -= (value + tvalue);
    grid->score += grid->values[ty][tx];

    return (MoveResult){ tx, ty, grid->values[ty][tx], change };
}

MoveResult peekGridMove(const Grid* grid, Move* move) {
    if (!validateGridMove(grid, move)) {
        return (MoveResult){ -1, -1, 0, 0 };
    }

    int value = grid->values[move->y][move->x];
    int tx = move->x + move->dir.dx * value;
    int ty = move->y + move->dir.dy * value;
    int tvalue = grid->values[ty][tx];

    int change = move->add ? value : -value;
    return (MoveResult){ tx, ty, abs(tvalue + change), change };
}

bool isGridSolved(const Grid* grid) {
    return grid->score == 0;
}

int getGridValue(const Grid* grid, int x, int y) {
    if (!inBounds(grid, x, y)) {
        return -1;
    }
    return grid->values[y][x];    
}
